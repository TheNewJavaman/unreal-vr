using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Globalization.NumberFormatting;
using Windows.Security.Cryptography;
using Windows.Storage;
using Windows.Storage.Pickers;
using WinRT;

namespace UnrealVRLauncher
{
    public sealed partial class MainWindow : INotifyPropertyChanged
    {
        private const string DEFAULT_FORMATTED_EXE = "Ex. [Game]\\Binaries\\Win64\\[Game]-Win64-Shipping.exe";

        public MainWindow()
        {
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
            Task.Factory.StartNew(() => GetProfiles());
        }

        private async void GetProfiles()
        {
            var localFolder = ApplicationData.Current.LocalFolder;
            var profileFolder = await localFolder.CreateFolderAsync("profiles", CreationCollisionOption.OpenIfExists);
            var profileFiles = await profileFolder.GetFilesAsync();
            foreach (var profileFile in profileFiles)
            {
                var text = await FileIO.ReadTextAsync(profileFile);
                dynamic model = JObject.Parse(text);
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
        private bool running = false;
        private bool ShowStart { get { return !running && ProfileSelected; } }
        private bool ShowStop { get { return running && ProfileSelected; } }

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

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            var profile = new ProfileModel();
            var newProfiles = new List<ProfileModel>(Profiles);
            newProfiles.Add(profile);
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
        }

        private void Copy_Click(object sender, RoutedEventArgs e)
        {
            var profile = Profile.Clone();
            Task.Factory.StartNew(() => profile.SaveToAppData());
            var newProfiles = new List<ProfileModel>(Profiles);
            newProfiles.Add(profile);
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
            dynamic model = JObject.Parse(text);
            var profile = new ProfileModel(Guid.NewGuid().ToString() + ".json", model);
            profile.SaveToAppData();
            var newProfiles = new List<ProfileModel>(Profiles);
            newProfiles.Add(profile);
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
    }

    public class ProfileModel : INotifyPropertyChanged
    {
        public ProfileModel()
        {
            Filename = Guid.NewGuid().ToString() + ".json";
            Task.Factory.StartNew(() => SaveToAppData());
        }

        public ProfileModel(string filename, dynamic model)
        {
            Filename = filename;
            if (model._Version == 1)
            {
                Name = model.Name;
                ShippingExe = model.ShippingExe;
                CommandLineArgs = model.CommandLineArgs;
                CmUnitsScale = model.CmUnitsScale;
                UsesFChunkedFixedUObjectArray = model.UsesFChunkedFixedUObjectArray;
                Uses422NamePool = model.Uses422NamePool;
                UsesFNamePool = model.UsesFNamePool;
                UsesDeferredSpawn = model.UsesDeferredSpawn;
            }
        }

        public async void SaveToAppData()
        {
            var localFolder = ApplicationData.Current.LocalFolder;
            var profileFolder = await localFolder.CreateFolderAsync("profiles", CreationCollisionOption.OpenIfExists);
            while (true)
            {
                try
                {
                    StorageFile profileFile = await profileFolder.CreateFileAsync(Filename, CreationCollisionOption.ReplaceExisting);
                    Save(profileFile);
                    break;
                }
                catch (Exception e)
                {
                    Console.WriteLine("Caught file exception: " + e);
                    Thread.Sleep(50);
                }
            }
        }

        public async void Save(StorageFile profileFile)
        {
            dynamic model = new
            {
                _Version = 1,
                Name,
                ShippingExe,
                CommandLineArgs,
                CmUnitsScale,
                UsesFChunkedFixedUObjectArray,
                Uses422NamePool,
                UsesFNamePool,
                UsesDeferredSpawn
            };
            string text = JsonConvert.SerializeObject(model);
            var buffer = CryptographicBuffer.ConvertStringToBinary(text, BinaryStringEncoding.Utf8);
            while (true)
            {
                try
                {
                    await FileIO.WriteBufferAsync(profileFile, buffer);
                    break;
                }
                catch (Exception e)
                {
                    Console.WriteLine("Caught file exception: " + e);
                    Thread.Sleep(50);
                }
            }
        }

        public string Filename { get; set; }
        public string Name { get; set; } = "Untitled";
        public string ShippingExe { get; set; } = "";
        public string CommandLineArgs { get; set; } = "";
        public float CmUnitsScale { get; set; } = 1.0f;
        public bool UsesFChunkedFixedUObjectArray { get; set; } = true;
        public bool Uses422NamePool { get; set; } = false;
        public bool UsesFNamePool { get; set; } = true;
        public bool UsesDeferredSpawn { get; set; } = false;

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public ProfileModel Clone() => new()
        {
            Name = Name + " (Copy)",
            ShippingExe = ShippingExe,
            CommandLineArgs = CommandLineArgs,
            CmUnitsScale = CmUnitsScale,
            UsesFChunkedFixedUObjectArray = UsesFChunkedFixedUObjectArray,
            Uses422NamePool = Uses422NamePool,
            UsesFNamePool = UsesFNamePool,
            UsesDeferredSpawn = UsesDeferredSpawn
        };

        public async void Delete()
        {
            var localFolder = ApplicationData.Current.LocalFolder;
            var profileFolder = await localFolder.CreateFolderAsync("profiles", CreationCollisionOption.OpenIfExists);
            while (true)
            {
                try
                {
                    StorageFile profileFile = await profileFolder.CreateFileAsync(Filename, CreationCollisionOption.ReplaceExisting);
                    await profileFile.DeleteAsync();
                    break;
                }
                catch (Exception e)
                {
                    Console.WriteLine("Caught file exception: " + e);
                    Thread.Sleep(50);
                }
            }
        }
    }
}