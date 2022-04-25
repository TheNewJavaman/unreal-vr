using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Globalization.NumberFormatting;
using Windows.Storage;
using Windows.Storage.Pickers;
using WinRT;
using Windows.Win32;
using Windows.Win32.System.Diagnostics.ToolHelp;
using Windows.Win32.System.Threading;
using Windows.Win32.System.Memory;
using Microsoft.Win32.SafeHandles;
using System.Windows.Input;
using Windows.System;
using Windows.Win32.Foundation;
using System.Threading;

namespace UnrealVR
{
    public sealed partial class MainWindow : INotifyPropertyChanged
    {
        private static readonly string DEFAULT_FORMATTED_EXE = "Ex. [Game]\\Binaries\\Win64\\[Game]-Win64-Shipping.exe";

        public MainWindow()
        {
            Title = "UnrealVR";
            InitializeComponent();
            var rounder = new IncrementNumberRounder
            {
                Increment = 0.001,
                RoundingAlgorithm = RoundingAlgorithm.RoundHalfUp
            };
            var formatter = new DecimalFormatter
            {
                FractionDigits = 3,
                NumberRounder = rounder
            };
            ScaleIncrement.NumberFormatter = formatter;
            GetProfiles();
        }

        private async void GetProfiles()
        {
            var localFolder = ApplicationData.Current.LocalFolder;
            var profileFolder = await localFolder.CreateFolderAsync("UnrealVRProfiles", CreationCollisionOption.OpenIfExists);
            var profileFiles = await profileFolder.GetFilesAsync();
            ProfileLastChanged = DateTime.Now;
            foreach (var profileFile in profileFiles)
            {
                var text = await FileIO.ReadTextAsync(profileFile);
                dynamic model;
                try
                {
                    model = JObject.Parse(text);
                } 
                catch (JsonReaderException)
                {
                    ShowError("Failed to parse game profile " + profileFile.DisplayName + "!");
                    continue;
                }
                var profile = new ProfileModel(profileFile.Name, model);
                var i = 0;
                while (i < ProfileSelector.Items.Count)
                {
                    if (profile.Name.CompareTo(Profiles[i].Name) < 0)
                    {
                        Profiles.Insert(i, profile);
                        break;
                    }
                    i++;
                }
                if (i == Profiles.Count)
                {
                    Profiles.Insert(i, profile);
                }
            }
        }

        private BindingList<ProfileModel> Profiles = new();
        private bool ProfileSelected = false;
        private ProfileModel Profile;
        private string FormattedExe = DEFAULT_FORMATTED_EXE;
        private SafeProcessHandle proc = new();
        private bool ShowStart { get { return proc.IsInvalid && ProfileSelected; } }
        private bool ShowStop { get { return !proc.IsInvalid && ProfileSelected; } }
        private readonly PipeServer Server = new();
        private DateTime ProfileLastChanged = DateTime.Now;

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private async void Start_Click(object sender, RoutedEventArgs e)
        {
            await Profile.WriteUMLProfile();
            if (!Start_Click_Unsafe())
            {
                ShowError("Couldn't create game process!");
                return;
            }
            InjectVR();
        }

        private unsafe bool Start_Click_Unsafe()
        {
            STARTUPINFOW startupInfo = new();
            startupInfo.cb = (uint)Marshal.SizeOf(startupInfo);
            var shippingDir = Profile.ShippingExe[..Profile.ShippingExe.LastIndexOf("\\")];
            var commandLineArr = (Profile.ShippingExe + " " + Profile.CommandLineArgs).ToCharArray();
            var commandLinePtr = GCHandle.Alloc(commandLineArr, GCHandleType.Pinned);
            PWSTR commandLine = new((char*)commandLinePtr.AddrOfPinnedObject().ToPointer());
            var ret = PInvoke.CreateProcess(null, commandLine, null, null, true, 0, null, shippingDir, startupInfo, out PROCESS_INFORMATION procInfo);
            commandLinePtr.Free();
            if (!ret) return false;
            proc = new SafeProcessHandle(procInfo.hProcess.Value, true);
            return true;
        }

        private async void Attach_Click(object sender, RoutedEventArgs e)
        {
            await Profile.WriteUMLProfile();
            var snapshotFlags = CREATE_TOOLHELP_SNAPSHOT_FLAGS.TH32CS_SNAPPROCESS | CREATE_TOOLHELP_SNAPSHOT_FLAGS.TH32CS_SNAPMODULE;
            var hSnapshot = PInvoke.CreateToolhelp32Snapshot_SafeHandle(snapshotFlags, 0);
            if (hSnapshot.IsInvalid)
            {
                ShowError("Couldn't create toolhelp snapshot");
                return;
            }
            PROCESSENTRY32 process = new();
            process.dwSize = (uint)Marshal.SizeOf(process);
            MODULEENTRY32 module = new();
            module.dwSize = (uint)Marshal.SizeOf(module);
            PInvoke.Process32First(hSnapshot, ref process);
            var exe = Profile.ShippingExe.Split('\\').Last();
            do
            {
                string filename = "";
                for (int i = 0; i < 260; i++)
                {
                    var character = (char)process.szExeFile[i].Value;
                    if (character == '\x00') break;
                    else filename += character;
                }
                if (filename == exe)
                {
                    var access = PROCESS_ACCESS_RIGHTS.PROCESS_CREATE_THREAD
                        | PROCESS_ACCESS_RIGHTS.PROCESS_TERMINATE
                        | PROCESS_ACCESS_RIGHTS.PROCESS_VM_OPERATION
                        | PROCESS_ACCESS_RIGHTS.PROCESS_VM_WRITE;
                    proc = new SafeProcessHandle(PInvoke.OpenProcess_SafeHandle(access, false, process.th32ProcessID).DangerousGetHandle(), true);
                    break;
                }
            } while (PInvoke.Process32Next(hSnapshot, ref process));
            if (proc.IsInvalid)
            {
                ShowError("Couldn't find game process!");
                return;
            }
            InjectVR();
        }

        private async void InjectVR()
        {
            NotifyPropertyChanged(nameof(ShowStart));
            NotifyPropertyChanged(nameof(ShowStop));
            SyncSettings();
            if (!await InjectDLL("UnrealEngineModLoader.dll")) return;
            if (!await InjectDLL("openxr_loader.dll")) return;
            if (!await InjectDLL("UnrealVRLoader.dll")) return;
            _ = Task.Run(CheckStopped);
        }

        private async void SyncSettings()
        {
            await Server.WaitForConnection();
            await Server.SendSettingChangeAsync(Setting.CmUnitsScale, Profile.CmUnitsScale);
        }

        private async Task<bool> InjectDLL(string name)
        {
            var installDir = Package.Current.InstalledLocation;
            var localDir = ApplicationData.Current.LocalFolder;
            var dllFile = await installDir.GetFileAsync(name);
            var dllCopy = await dllFile.CopyAsync(localDir, name, NameCollisionOption.ReplaceExisting);
            if (!WriteDLLSortOfSafely(dllCopy)) return false;
            return true;
        }

        unsafe private bool WriteDLLSortOfSafely(StorageFile dll)
        {
            var loc = PInvoke.VirtualAllocEx(proc, null, PInvoke.MAX_PATH, VIRTUAL_ALLOCATION_TYPE.MEM_COMMIT | VIRTUAL_ALLOCATION_TYPE.MEM_RESERVE, PAGE_PROTECTION_FLAGS.PAGE_READWRITE);
            if (loc == null)
            {
                ShowError("Couldn't allocate memory in the game process!");
                return false;
            }
            var dllPath = dll.Path.Select(c => (byte)c).ToList();
            dllPath.Add(0x00);
            var dllPathArr = dllPath.ToArray();
            var handle = GCHandle.Alloc(dllPathArr, GCHandleType.Pinned);
            if (!PInvoke.WriteProcessMemory(proc, loc, handle.AddrOfPinnedObject().ToPointer(), (uint)dllPathArr.Length, null))
            {
                ShowError("Couldn't write memory in the game process!");
                return false;
            };
            handle.Free();
            var loadLibrary = PInvoke.GetProcAddress(PInvoke.GetModuleHandle("kernel32.dll"), "LoadLibraryA");
            if (loadLibrary.IsNull)
            {
                ShowError("Couldn't get the address of LoadLibraryA!");
                return false;
            };
            var hThread = PInvoke.CreateRemoteThread(proc, null, 0, loadLibrary.CreateDelegate<LPTHREAD_START_ROUTINE>(), loc, 0, null);
            if (hThread.IsInvalid)
            {
                ShowError("Couldn't create a remote thread in the game process!");
                return false;
            };
            hThread.Close();
            return true;
        }

        private void CheckStopped()
        {
            while (!proc.IsInvalid)
            {
                if (!PInvoke.GetExitCodeProcess(proc, out uint lpExitCode))
                {
                    ShowError("Failed to get exit code for game process!");
                }
                if (lpExitCode != 259) break; // STILL_ACTIVE = 259
                Thread.Sleep(500);
            }
            ResetProcess();
        }

        private void Stop_Click(object sender, RoutedEventArgs e)
        {
            if (!proc.IsInvalid)
                PInvoke.TerminateProcess(proc, 0);
            ResetProcess();
        }

        private void ResetProcess()
        {
            Server.Disconnect();
            if (!proc.IsInvalid) 
                proc.Close();
            proc = new();
            DispatcherQueue.TryEnqueue(() =>
            {
                NotifyPropertyChanged(nameof(ShowStart));
                NotifyPropertyChanged(nameof(ShowStop));
            });
        }

        private void Profile_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ProfileLastChanged = DateTime.Now;
            var comboBox = sender as ComboBox;
            Profile = comboBox.SelectedItem as ProfileModel;
            NotifyPropertyChanged(nameof(Profile));
            ProfileSelected = true;
            NotifyPropertyChanged(nameof(ProfileSelected));
            NotifyPropertyChanged(nameof(ShowStart));
            NotifyPropertyChanged(nameof(ShowStop));
            if (Profile == null) return;
            else if (Profile.ShippingExe == "") FormattedExe = DEFAULT_FORMATTED_EXE;
            else FormattedExe = Profile.ShippingExe;
            NotifyPropertyChanged(nameof(FormattedExe));
        }

        /** Credit: https://github.com/microsoft/microsoft-ui-xaml/issues/4100#issuecomment-774346918 */
        private async void ExeSelect_Clicked(object sender, RoutedEventArgs e)
        {
            var filePicker = new FileOpenPicker();
            var hwnd = this.As<IWindowNative>().WindowHandle;
            var initializeWithWindow = filePicker.As<IInitializeWithWindow>();
            initializeWithWindow.Initialize(hwnd);
            filePicker.FileTypeFilter.Add(".exe");
            var result = await filePicker.PickSingleFileAsync();
            if (result == null) return;
            Profile.ShippingExe = result.Path;
            FormattedExe = result.Path;
            NotifyPropertyChanged(nameof(FormattedExe));
            SaveProfile(Profile);
        }

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            ProfileLastChanged = DateTime.Now;
            var profile = new ProfileModel();
            var newProfiles = new List<ProfileModel>(Profiles) { profile };
            Profiles = new BindingList<ProfileModel>(newProfiles.OrderBy(profile => profile.Name).ToList());
            NotifyPropertyChanged(nameof(Profiles));
            ProfileSelector.SelectedItem = profile;
        }

        private void Copy_Click(object sender, RoutedEventArgs e)
        {
            ProfileLastChanged = DateTime.Now;
            var profile = Profile.Clone();
            SaveProfile(profile);
            var newProfiles = new List<ProfileModel>(Profiles) { profile };
            Profiles = new BindingList<ProfileModel>(newProfiles.OrderBy(profile => profile.Name).ToList());
            NotifyPropertyChanged(nameof(Profiles));
            ProfileSelector.SelectedItem = profile;
        }

        private async void Remove_Click(object sender, RoutedEventArgs e)
        {
            ProfileLastChanged = DateTime.Now;
            await Profile.Delete();
            var i = ProfileSelector.SelectedIndex;
            var newList = new List<ProfileModel>(Profiles);
            newList.RemoveAt(i);
            Profiles = new BindingList<ProfileModel>(newList);
            NotifyPropertyChanged(nameof(Profiles));
            if (i == 0)
            {
                if (newList.Count > 0)
                {
                    ProfileSelector.SelectedIndex = 0;
                }
                else
                {
                    ProfileSelector.SelectedIndex = -1;
                    ProfileSelected = false;
                    NotifyPropertyChanged(nameof(ProfileSelected));
                    NotifyPropertyChanged(nameof(ShowStart));
                    NotifyPropertyChanged(nameof(ShowStop));
                    NameBox.Text = "";
                    ArgsBox.Text = "";
                    UsesFChunkedFixedUObjectArraySwitch.IsOn = false;
                    Uses422NamePoolSwitch.IsOn = false;
                    UsesFNamePoolSwitch.IsOn = false;
                    UsesDeferredSpawnSwitch.IsOn = false;
                    ScaleIncrement.Value = 1.0f;
                    FormattedExe = DEFAULT_FORMATTED_EXE;
                    NotifyPropertyChanged(nameof(FormattedExe));
                }
            }
            else
            {
                ProfileSelector.SelectedIndex = i - 1;
            }
        }

        private async void Import_Click(object sender, RoutedEventArgs e)
        {
            ProfileLastChanged = DateTime.Now;
            var filePicker = new FileOpenPicker();
            var hwnd = this.As<IWindowNative>().WindowHandle;
            var initializeWithWindow = filePicker.As<IInitializeWithWindow>();
            initializeWithWindow.Initialize(hwnd);
            filePicker.FileTypeFilter.Add(".json");
            var result = await filePicker.PickSingleFileAsync();
            if (result == null) return;
            var text = await FileIO.ReadTextAsync(result);
            dynamic model;
            try
            {
                model = JObject.Parse(text);
            }
            catch (JsonReaderException)
            {
                ShowError("Failed to parse game profile!");
                return;
            }
            var profile = new ProfileModel(Guid.NewGuid().ToString() + ".json", model);
            await profile.SaveToAppData();
            var newProfiles = new List<ProfileModel>(Profiles) { profile };
            Profiles = new BindingList<ProfileModel>(newProfiles.OrderBy(profile => profile.Name).ToList());
            NotifyPropertyChanged(nameof(Profiles));
            ProfileSelector.SelectedItem = profile;
        }

        private async void Export_Click(object sender, RoutedEventArgs e)
        {
            var filePicker = new FileSavePicker();
            var hwnd = this.As<IWindowNative>().WindowHandle;
            var initializeWithWindow = filePicker.As<IInitializeWithWindow>();
            initializeWithWindow.Initialize(hwnd);
            filePicker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            filePicker.FileTypeChoices.Add("JSON", new List<string>() { ".json" });
            var result = await filePicker.PickSaveFileAsync();
            if (result == null) return;
            SaveProfile(Profile);
        }

        private void Name_LostFocus(object sender, RoutedEventArgs e)
        {
            var profile = Profile;
            if (profile == null) return;
            var textBox = sender as TextBox;
            profile.Name = textBox.Text;
            profile.NotifyPropertyChanged(nameof(profile.Name));
            SaveProfile(profile);
            var newList = (new List<ProfileModel>(Profiles)).OrderBy(it => it.Name).ToList();
            Profiles = new BindingList<ProfileModel>(newList);
            NotifyPropertyChanged(nameof(Profiles));
            ProfileSelector.SelectedItem = profile;
        }

        private void Args_LostFocus(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var textBox = sender as TextBox;
            Profile.CommandLineArgs = textBox.Text;
            Profile.NotifyPropertyChanged(nameof(Profile.CommandLineArgs));
            SaveProfile(Profile);
        }

        private void UsesFChunkedFixedUObjectArray_LostFocus(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesFChunkedFixedUObjectArray = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesFChunkedFixedUObjectArray));
            SaveProfile(Profile);
        }

        private void Uses422NamePool_LostFocus(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.Uses422NamePool = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.Uses422NamePool));
            SaveProfile(Profile);
        }

        private void UsesFNamePool_LostFocus(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesFNamePool = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesFNamePool));
            SaveProfile(Profile);
        }

        private void UsesDeferredSpawn_LostFocus(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesDeferredSpawn = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesDeferredSpawn));
            SaveProfile(Profile);
        }

        private async void ScaleIncrement_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs e)
        {
            if (Profile == null) return;
            var numberBox = sender as NumberBox;
            Profile.CmUnitsScale = (float)numberBox.Value;
            Profile.NotifyPropertyChanged(nameof(Profile.CmUnitsScale));
            SaveProfile(Profile);
            if (Server.IsConnected)
                await Server.SendSettingChangeAsync(Setting.CmUnitsScale, (float)numberBox.Value);
        }

        private async void SaveProfile(ProfileModel profile)
        {
            var now = DateTime.Now;
            if (now - ProfileLastChanged > TimeSpan.FromSeconds(1))
            {
                ProfileLastChanged = now;
                await profile.SaveToAppData();
            }
        }

        private void ShowError(string message)
        {
            var dialog = new ContentDialog
            {
                Title = "Error",
                Content = message + "\n\nIf this is unexpected, please report this issue on GitHub.",
                PrimaryButtonText = "Report Issue",
                PrimaryButtonCommand = new ReportCommand(),
                CloseButtonText = "Ok",
                DefaultButton = ContentDialogButton.Close,
                XamlRoot = Content.XamlRoot
            };
            DispatcherQueue.TryEnqueue(() => _ = dialog.ShowAsync());
        }
    }

    [ComImport]
    [Guid("3E68D4BD-7135-4D10-8018-9FB6D9F33FA1")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IInitializeWithWindow
    {
        void Initialize(IntPtr hwnd);
    }

    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("EECDBF0E-BAE9-4CB6-A68E-9598E1CB57BB")]
    internal interface IWindowNative
    {
        IntPtr WindowHandle { get; }
    }

    public class ReportCommand : ICommand
    {
        public event EventHandler CanExecuteChanged;

        public bool CanExecute(object parameter) => true;

        public async void Execute(object parameter)
        {
            await Launcher.LaunchUriAsync(new Uri("https://github.com/TheNewJavaman/unreal-vr/issues/new"));
        }
    }
}