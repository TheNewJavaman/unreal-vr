// ReSharper disable BuiltInTypeReferenceStyle

using System.Runtime.InteropServices;
using RGiesecke.DllExport;
using Tools.DotNETCommon;
using UnrealBuildTool;

namespace UnrealModuleInterface;

internal record UnrealModuleInfo(
    string[] PublicIncludePaths,
    string[] PrivateIncludePaths,
    string[] PublicModuleDependencies,
    string[] PrivateModuleDependencies)
{
    private const UInt16 Terminator = 0x0000;

    internal UInt16[] ToUInt16Array() => PublicIncludePaths.SelectMany(StringToUInt16Array).Append(Terminator)
        .Concat(PrivateIncludePaths.SelectMany(StringToUInt16Array).Append(Terminator))
        .Concat(PublicModuleDependencies.SelectMany(StringToUInt16Array).Append(Terminator))
        .Concat(PrivateModuleDependencies.SelectMany(StringToUInt16Array).Append(Terminator))
        .ToArray();

    private static IEnumerable<UInt16> StringToUInt16Array(string In) =>
        In.Select(It => (UInt16) It).Append(Terminator);
}

internal class UnrealVrTargetRules : TargetRules
{
    private static readonly TargetInfo MyTargetInfo = new(
        "FakeUnrealVRTarget",
        UnrealTargetPlatform.Win64,
        UnrealTargetConfiguration.Shipping,
        "",
        null,
        new CommandLineArguments(Array.Empty<string>())
    );

    internal UnrealVrTargetRules() : base(MyTargetInfo)
    {
    }
}

internal static class NativeInterface
{
    private static readonly HashSet<GCHandle> MyPins = new();
    private static readonly Dictionary<string, ModuleRules> MyModuleMap = new();

    static NativeInterface()
    {
        var MyReadOnlyTargetRules = new ReadOnlyTargetRules(new UnrealVrTargetRules());
        {ModuleMapEntries}
    }

    [DllExport("borrow_unreal_module_info", CallingConvention = CallingConvention.Cdecl)]
    private static IntPtr BorrowUnrealModuleInfo(string Name)
    {
        var Module = MyModuleMap[new string(Name)];
        var Info = new UnrealModuleInfo(
            Module.PublicIncludePaths.ToArray(),
            Module.PrivateIncludePaths.ToArray(),
            Module.PublicDependencyModuleNames.ToArray(),
            Module.PrivateDependencyModuleNames.ToArray()
        );
        var Pin = GCHandle.Alloc(Info.ToUInt16Array(), GCHandleType.Pinned);
        MyPins.Add(Pin);
        return Pin.AddrOfPinnedObject();
    }

    [DllExport("release_unreal_module_info", CallingConvention = CallingConvention.Cdecl)]
    private static void ReleaseUnrealModuleInfo(IntPtr InfoPtr)
    {
        MyPins.RemoveWhere(It =>
        {
            if (It.AddrOfPinnedObject() != InfoPtr) return false;
            It.Free();
            return true;
        });
    }
}
