using System;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace UnrealVRLauncher
{
    public sealed partial class MainWindow
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void NavigationView_ItemInvoked(NavigationView sender, NavigationViewItemInvokedEventArgs args)
        {
            
            Type pageType;
            var item = (NavigationViewItem) sender.SelectedItem;
            if (item == GeneralItem)
            {
                pageType = typeof(GeneralPage);
            }
            else if (item == ProfileItem)
            {
                pageType = typeof(ProfilePage);
            }
            else
            {
                pageType = typeof(AboutPage);
            }
            var navOptions = new FrameNavigationOptions
            {
                TransitionInfoOverride = args.RecommendedNavigationTransitionInfo
            };
            ContentFrame.NavigateToType(pageType, null, navOptions);
            sender.Header = item.Content;
        }
    }
}