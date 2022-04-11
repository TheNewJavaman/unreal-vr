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
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Globalization.NumberFormatting;
using Windows.Storage;
using Windows.Storage.Pickers;
using WinRT;
using static UnrealVR.Native;

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
            FOVScale.NumberFormatter = formatter;
            Task.Factory.StartNew(() => GetProfiles());
        }

        private async void GetProfiles()
        {
            var localFolder = ApplicationData.Current.LocalFolder;
            var profileFolder = await localFolder.CreateFolderAsync("UnrealVRProfiles", CreationCollisionOption.OpenIfExists);
            var profileFiles = await profileFolder.GetFilesAsync();
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
                    DispatcherQueue.TryEnqueue(() => _ = ShowError("Failed to parse game profile " + profileFile.DisplayName + "!"));
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
        private IntPtr proc = IntPtr.Zero;
        private bool ShowStart { get { return proc == IntPtr.Zero && ProfileSelected; } }
        private bool ShowStop { get { return proc != IntPtr.Zero && ProfileSelected; } }

        private PipeServer server;
        private float FOVMultiplier { get; set; } = 1.0f;

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private void Profile_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
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
            Profile.SaveToAppData();
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

        private async void Add_Click(object sender, RoutedEventArgs e)
        {
            var profile = new ProfileModel();
            var newProfiles = new List<ProfileModel>(Profiles) { profile };
            Profiles = new BindingList<ProfileModel>(newProfiles.OrderBy(profile => profile.Name).ToList());
            NotifyPropertyChanged(nameof(Profiles));
            ProfileSelector.SelectedItem = profile;
        }

        private void Name_TextChanged(object sender, TextChangedEventArgs e)
        {
            var profile = Profile;
            if (profile == null) return;
            var textBox = sender as TextBox;
            profile.Name = textBox.Text;
            profile.NotifyPropertyChanged(nameof(profile.Name));
            Task.Factory.StartNew(() => profile.SaveToAppData());
            var newList = (new List<ProfileModel>(Profiles)).OrderBy(it => it.Name).ToList();
            Profiles = new BindingList<ProfileModel>(newList);
            NotifyPropertyChanged(nameof(Profiles));
            ProfileSelector.SelectedItem = profile;
        }

        private void Args_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (Profile == null) return;
            var textBox = sender as TextBox;
            Profile.CommandLineArgs = textBox.Text;
            Profile.NotifyPropertyChanged(nameof(Profile.CommandLineArgs));
            Task.Factory.StartNew(() => Profile.SaveToAppData());
        }

        private void UsesFChunkedFixedUObjectArray_Toggled(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesFChunkedFixedUObjectArray = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesFChunkedFixedUObjectArray));
            Task.Factory.StartNew(() => Profile.SaveToAppData());
        }

        private void Uses422NamePool_Toggled(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.Uses422NamePool = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.Uses422NamePool));
            Task.Factory.StartNew(() => Profile.SaveToAppData());
        }

        private void UsesFNamePool_Toggled(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesFNamePool = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesFNamePool));
            Task.Factory.StartNew(() => Profile.SaveToAppData());
        }

        private void UsesDeferredSpawn_Toggled(object sender, RoutedEventArgs e)
        {
            if (Profile == null) return;
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesDeferredSpawn = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesDeferredSpawn));
            Task.Factory.StartNew(() => Profile.SaveToAppData());
        }

        private void ScaleIncrement_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            if (Profile == null) return;
            var numberBox = sender as NumberBox;
            Profile.CmUnitsScale = (float)numberBox.Value;
            Profile.NotifyPropertyChanged(nameof(Profile.CmUnitsScale));
            Task.Factory.StartNew(() => Profile.SaveToAppData());
            if (server != null && server.IsConnected)
                server.SendSettingChange(Setting.CmUnitsScale, (float)numberBox.Value);
        }

        private void FOVScale_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            var numberBox = sender as NumberBox;
            FOVMultiplier = (float)numberBox.Value;
            NotifyPropertyChanged(nameof(FOVMultiplier));
            if (server != null && server.IsConnected)
                server.SendSettingChange(Setting.FOVScale, (float)numberBox.Value);
        }

        private void Copy_Click(object sender, RoutedEventArgs e)
        {
            var profile = Profile.Clone();
            Task.Factory.StartNew(() => profile.SaveToAppData());
            var newProfiles = new List<ProfileModel>(Profiles) { profile };
            Profiles = new BindingList<ProfileModel>(newProfiles.OrderBy(profile => profile.Name).ToList());
            NotifyPropertyChanged(nameof(Profiles));
            ProfileSelector.SelectedItem = profile;
        }

        private void Remove_Click(object sender, RoutedEventArgs e)
        {
            Task.Factory.StartNew(() => Profile.Delete());
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
                    FOVScale.Value = 1.0f;
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
                await ShowError("Failed to parse game profile!");
                return;
            }
            var profile = new ProfileModel(Guid.NewGuid().ToString() + ".json", model);
            profile.SaveToAppData();
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
            Profile.Save(result);
        }

        private async void Start_Click(object sender, RoutedEventArgs e)
        {
            await Profile.WriteUMLProfile();
            var startupInfo = new STARTUPINFO();
            startupInfo.cb = Marshal.SizeOf(startupInfo);
            var shippingDir = Profile.ShippingExe[..Profile.ShippingExe.LastIndexOf("\\")];
            if (!CreateProcess(null, Profile.ShippingExe + " " + Profile.CommandLineArgs, IntPtr.Zero, IntPtr.Zero, true, 0, IntPtr.Zero, shippingDir, ref startupInfo, out PROCESS_INFORMATION procInfo))
            {
                await ShowError("Failed to create game process!");
                return;
            }
            if (!await InjectDLL("UnrealEngineModLoader.dll", procInfo)) return;
            if (!await InjectDLL("UnrealVRLoader.dll", procInfo)) return;
            if (!await InjectDLL("openxr_loader.dll", procInfo)) return;
            if (!CloseHandle(procInfo.hThread))
            {
                await ShowError("Failed to close process thread handle!");
            }
            proc = procInfo.hProcess;
            NotifyPropertyChanged(nameof(ShowStart));
            NotifyPropertyChanged(nameof(ShowStop));
            server = new PipeServer();
            server.Start();
            server.SendSettingChange(Setting.CmUnitsScale, Profile.CmUnitsScale);
            server.SendSettingChange(Setting.FOVScale, FOVMultiplier);
            //_ = Task.Factory.StartNew(CheckStopped);
        }

        private async Task<bool> InjectDLL(string name, PROCESS_INFORMATION procInfo)
        {
            var installDir = Package.Current.InstalledLocation;
            var localDir = ApplicationData.Current.LocalFolder;
            var dllFile = await installDir.GetFileAsync(name);
            var dllCopy = await dllFile.CopyAsync(localDir, name, NameCollisionOption.ReplaceExisting);
            var loc = VirtualAllocEx(procInfo.hProcess, IntPtr.Zero, MAX_PATH, (uint)AllocationType.Commit | (uint)AllocationType.Reserve, (uint)MemoryProtection.ReadWrite);
            if (loc == IntPtr.Zero)
            {
                await ShowError("Failed to allocate memory in game process!");
                return false;
            }
            var dllPath = Encoding.ASCII.GetBytes(dllCopy.Path);
            if (!WriteProcessMemory(procInfo.hProcess, loc, dllPath, dllPath.Length + 1, out _))
            {
                await ShowError("Failed to write DLL path to memory!");
                return false;
            }
            var loadLibrary = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
            var hThread = CreateRemoteThread(procInfo.hProcess, IntPtr.Zero, 0, loadLibrary, loc, 0, out _);
            if (hThread == IntPtr.Zero)
            {
                await ShowError("Failed to create remote thread!");
                return false;
            }
            if (!CloseHandle(hThread))
            {
                await ShowError("Failed to close DLL thread handle!");
                return false;
            }
            return true;
        }

        private async void CheckStopped()
        {
            while (true)
            {
                if (!GetExitCodeProcess(proc, out UIntPtr lpExitCode))
                {
                    await ShowError("Failed to get exit code process!");
                    return;
                }
                if (lpExitCode.ToUInt32() != STILL_ACTIVE)
                {
                    server.Stop();
                    await ResetProcess();
                    return;
                }
                Thread.Sleep(500);
            }
        }

        private async void Stop_Click(object sender, RoutedEventArgs e)
        {
            server.Stop();
            TerminateProcess(proc, SUCCESS);
            await ResetProcess();
        }

        private async Task ResetProcess()
        {
            if (!CloseHandle(proc))
            {
                await ShowError("Failed to close process handle!");
            }
            proc = IntPtr.Zero;
            NotifyPropertyChanged(nameof(ShowStart));
            NotifyPropertyChanged(nameof(ShowStop));
        }

        private async Task ShowError(string message, bool shouldReport = true)
        {
            var dialog = new ContentDialog
            {
                Title = "Unexpected Error",
                Content = shouldReport ? message + "\n\nPlease report this issue on GitHub." : message,
                CloseButtonText = "Ok",
                DefaultButton = ContentDialogButton.Close,
                XamlRoot = Content.XamlRoot
            };
            await dialog.ShowAsync();
        }
    }
}