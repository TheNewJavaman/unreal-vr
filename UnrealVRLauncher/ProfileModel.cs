using Newtonsoft.Json;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Security.Cryptography;
using Windows.Storage;

namespace UnrealVRLauncher
{
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
