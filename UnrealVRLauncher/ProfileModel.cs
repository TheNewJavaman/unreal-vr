using Newtonsoft.Json;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Security.Cryptography;
using Windows.Storage;

namespace UnrealVR
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
            if (model._Version == 1 || model._Version == 2)
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
            if (model._Version == 2)
            {
                FOVScale = model.FOVScale;
            }
        }

        public async void SaveToAppData()
        {
            var localFolder = ApplicationData.Current.LocalFolder;
            var profileFolder = await localFolder.CreateFolderAsync("UnrealVRProfiles", CreationCollisionOption.OpenIfExists);
            while (true)
            {
                try
                {
                    StorageFile profileFile = await profileFolder.CreateFileAsync(Filename, CreationCollisionOption.OpenIfExists);
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
                _Version = 2,
                Name,
                ShippingExe,
                CommandLineArgs,
                CmUnitsScale,
                UsesFChunkedFixedUObjectArray,
                Uses422NamePool,
                UsesFNamePool,
                UsesDeferredSpawn,
                FOVScale
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
        public float FOVScale { get; set; } = 1.0f;

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
            var profileFolder = await localFolder.CreateFolderAsync("UnrealVRProfiles", CreationCollisionOption.OpenIfExists);
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

private static readonly string UML_PROFILE_FORMAT = @"# An UnrealModLoader profile for {0} auto-generated by UnrealVR
[GameInfo]
IsUsingFChunkedFixedUObjectArray={1}
IsUsing4_22={2}
UsesFNamePool={3}
IsUsingDeferedSpawn={4}";
private static readonly string UML_CONFIG_FORMAT = @"# An UnrealModLoader config auto-generated by UnrealVR
[DEBUG]
UseConsole={0}";

        public async Task WriteUMLProfile()
        {
            var name = ShippingExe.Split('\\')[^1][..^4];
            var filename = name + ".profile";
            var localFolder = ApplicationData.Current.LocalFolder;
            
            var profileFolder = await localFolder.CreateFolderAsync("UnrealModLoaderProfiles", CreationCollisionOption.OpenIfExists);
            var profileFile = await profileFolder.CreateFileAsync(filename, CreationCollisionOption.OpenIfExists);
            var profileContent = string.Format(
                UML_PROFILE_FORMAT,
                name,
                UsesFChunkedFixedUObjectArray ? "1" : "0",
                Uses422NamePool ? "1" : "0",
                UsesFNamePool ? "1" : "0",
                UsesDeferredSpawn ? "1" : "0"
            );
            var profileBuffer = CryptographicBuffer.ConvertStringToBinary(profileContent, BinaryStringEncoding.Utf8);
            await FileIO.WriteBufferAsync(profileFile, profileBuffer);

            var configFile = await localFolder.CreateFileAsync("ModLoaderInfo.ini", CreationCollisionOption.OpenIfExists);
            var configContent = string.Format(UML_CONFIG_FORMAT, "1"); // TODO: Enable/disable UML console
            var configBuffer = CryptographicBuffer.ConvertStringToBinary(configContent, BinaryStringEncoding.Utf8);
            await FileIO.WriteBufferAsync(configFile, configBuffer);
        }
    }
}
