using Microsoft.UI.Xaml.Controls;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.CompilerServices;

namespace UnrealVRLauncher
{
    public sealed partial class MainWindow : INotifyPropertyChanged
    {
        public MainWindow()
        {
            InitializeComponent();
            Profiles.Add(new ProfileModel("Ghostrunner", "Ghostrunner-1.json", "C:\\Ghostrunner\\Shipping.exe", "-dx11", 1.1f));
            Profiles.Add(new ProfileModel("Borderlands 3", "Borderlands 3-1.json", "C:\\Borderlands 3\\Shipping.exe", "-dx11", 1.1f));
            Profiles.Add(new ProfileModel("Astroneer", "Astroneer-1.json", "C:\\Astroneer\\Shipping.exe", "-dx11", 1.1f));
        }

        public ObservableCollection<ProfileModel> Profiles = new();
        public ProfileModel Profile;

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var comboBox = sender as ComboBox;
            Profile = comboBox.SelectedItem as ProfileModel;
            NotifyPropertyChanged(nameof(Profile));
        }
    }

    public class ProfileModel
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

        public string FormatCmUnitsScale()
        {
            return CmUnitsScale.ToString("F3", CultureInfo.InvariantCulture);
        }
    }
}