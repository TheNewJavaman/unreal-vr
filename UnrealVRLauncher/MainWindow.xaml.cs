using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
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
                Profiles.Add(profile);
            }
        }

        private BindingList<ProfileModel> Profiles = new();
        private bool ProfileSelected = false;
        private ProfileModel Profile;
        private string FormattedExe = DEFAULT_FORMATTED_EXE;
        private bool Started = false;
        private bool Stopped = true;

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
            if (Profile.ShippingExe == "") FormattedExe = DEFAULT_FORMATTED_EXE;
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
            Profiles.Add(profile);
            //ProfileSelector.SelectedItem = profile;
        }

        private void Name_TextChanged(object sender, TextChangedEventArgs e)
        {
            var textBox = sender as TextBox;
            Profile.Name = textBox.Text;
            Profile.NotifyPropertyChanged(nameof(Profile.Name));
            Task.Factory.StartNew(() => Profile.Save());
        }

        private void Args_TextChanged(object sender, TextChangedEventArgs e)
        {
            var textBox = sender as TextBox;
            Profile.CommandLineArgs = textBox.Text;
            Profile.NotifyPropertyChanged(nameof(Profile.CommandLineArgs));
            Task.Factory.StartNew(() => Profile.Save());
        }

        private void UsesFChunkedFixedUObjectArray_Toggled(object sender, RoutedEventArgs e)
        {
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesFChunkedFixedUObjectArray = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesFChunkedFixedUObjectArray));
            Task.Factory.StartNew(() => Profile.Save());
        }

        private void Uses422NamePool_Toggled(object sender, RoutedEventArgs e)
        {
            var toggleSwitch = sender as ToggleSwitch;
            Profile.Uses422NamePool = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.Uses422NamePool));
            Task.Factory.StartNew(() => Profile.Save());
        }

        private void UsesFNamePool_Toggled(object sender, RoutedEventArgs e)
        {
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesFNamePool = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesFNamePool));
            Task.Factory.StartNew(() => Profile.Save());
        }

        private void UsesDeferredSpawn_Toggled(object sender, RoutedEventArgs e)
        {
            var toggleSwitch = sender as ToggleSwitch;
            Profile.UsesDeferredSpawn = toggleSwitch.IsOn;
            Profile.NotifyPropertyChanged(nameof(Profile.UsesDeferredSpawn));
            Task.Factory.StartNew(() => Profile.Save());
        }

        private void ScaleIncrement_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            var numberBox = sender as NumberBox;
            Profile.CmUnitsScale = (float) numberBox.Value;
            Profile.NotifyPropertyChanged(nameof(Profile.CmUnitsScale));
            Task.Factory.StartNew(() => Profile.Save());
        }
    }

    public class ProfileModel : INotifyPropertyChanged
    {
        public ProfileModel()
        {
            Filename = Guid.NewGuid().ToString() + ".json";
            Task.Factory.StartNew(() => Save());
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

        public async void Save()
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
            var localFolder = ApplicationData.Current.LocalFolder;
            var profileFolder = await localFolder.CreateFolderAsync("profiles", CreationCollisionOption.OpenIfExists);
            var profileFile = await profileFolder.CreateFileAsync(Filename, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteBufferAsync(profileFile, buffer);
        }

        public string Filename { get; set; }
        private string _name = "Untitled";
        public string Name
        {
            get { return _name; }
            set { _name = value; NotifyPropertyChanged(nameof(Name)); }
        }
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
    }
}