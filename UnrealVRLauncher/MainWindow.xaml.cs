using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Windows.Globalization.NumberFormatting;
using Windows.Storage.Pickers;
using WinRT;

namespace UnrealVRLauncher
{
    public sealed partial class MainWindow : INotifyPropertyChanged
    {
        public static string DEFAULT_FORMATTED_EXE = "Ex. [Game]\\Binaries\\Win64\\[Game]-Win64-Shipping.exe";

        public MainWindow()
        {
            InitializeComponent();
            Profiles.Add(new ProfileModel("Ghostrunner", "Ghostrunner-1.json", "C:\\Ghostrunner\\Shipping.exe", "-dx11", 1.1f));
            Profiles.Add(new ProfileModel("Borderlands 3", "Borderlands 3-1.json", "C:\\Borderlands 3\\Shipping.exe", "-dx11", 1.1f));
            Profiles.Add(new ProfileModel("Astroneer", "Astroneer-1.json", "", "-dx11", 1.1f));
            var rounder = new IncrementNumberRounder();
            rounder.Increment = 0.001;
            rounder.RoundingAlgorithm = RoundingAlgorithm.RoundHalfUp;
            var formatter = new DecimalFormatter();
            formatter.FractionDigits = 3;
            formatter.NumberRounder = rounder;
            ScaleIncrement.NumberFormatter = formatter;
        }

        public ObservableCollection<ProfileModel> Profiles = new();
        public bool ProfileSelected = false;
        public ProfileModel Profile;
        public string FormattedExe = DEFAULT_FORMATTED_EXE;
        public bool Started = false;
        public bool Stopped = true;

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
        private async void ExeSelect_Clicked(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
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

        private void ScaleDecrement_Click(object sender, RoutedEventArgs e)
        {
            Profile.CmUnitsScale -= 0.01f;
            Profile.NotifyPropertyChanged(nameof(Profile.CmUnitsScale));
        }

        private void ScaleIncrement_Click(object sender, RoutedEventArgs e)
        {
            Profile.CmUnitsScale += 0.01f;
            Profile.NotifyPropertyChanged(nameof(Profile.CmUnitsScale));
        }
    }

    public class ProfileModel : INotifyPropertyChanged
    {
        public ProfileModel(string name, string filename, string shippingExe, string commandLineArgs, float cmUnitsScale)
        {
            Name = name;
            Filename = filename;
            ShippingExe = shippingExe;
            CommandLineArgs = commandLineArgs;
            CmUnitsScale = cmUnitsScale;
        }

        public string Name { get; set; } = "Untitled";
        public string Filename { get; set; } = "";
        public string ShippingExe { get; set; } = "";
        public string CommandLineArgs { get; set; } = "";
        public float CmUnitsScale { get; set; } = 1.0f;
        public bool UsesFChunkedFixedUObjectArray { get; set; } = true;
        public bool UsesFNamePool { get; set; } = true;
        public bool Uses422NamePool { get; set; } = false;
        public bool UsesDeferredSpawn { get; set; } = false;

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}