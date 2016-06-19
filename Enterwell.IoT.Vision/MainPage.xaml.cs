using Enterwell_IoT_Vision_Algorithm;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Imaging;
using Windows.Media.Capture;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Enterwell.IoT.Vision
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        Windows.Media.Capture.MediaCapture captureManager;
        VisionService visionService;

        MediaCapture _mediaCapture;
        bool _isInitialized = false;

        public MainPage()
        {
            this.InitializeComponent();

            this.Loaded += MainPage_Loaded;

        }

        private async void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            this.visionService = new VisionService();
            var settings = new MediaCaptureInitializationSettings()
            {
                PhotoCaptureSource = PhotoCaptureSource.VideoPreview,
                VideoDeviceId = "USB\\VID_046D&PID_C52B\\5&ECB7860&0&3",
                StreamingCaptureMode = StreamingCaptureMode.Video

            };

            captureManager = new MediaCapture();
            await captureManager.InitializeAsync();
            
            var timer = new DispatcherTimer()
            {
                Interval = TimeSpan.FromMilliseconds(50)
            };
            
            timer.Tick += Timer_Tick;

            timer.Start();
        }

        private async void Timer_Tick(object sender, object e)
        {
            await CapturePicture();
        }

        private volatile bool isCaptureDone = true;

        private async Task CapturePicture()
        {
            if (!isCaptureDone)
                return;

            isCaptureDone = false;

            var stream = new InMemoryRandomAccessStream();

            await captureManager.CapturePhotoToStreamAsync(Windows.Media.MediaProperties.ImageEncodingProperties.CreateJpeg(), stream);

            var decoder = await BitmapDecoder.CreateAsync(stream);
            
            using (var memStream = new InMemoryRandomAccessStream())
            {
                var encoder = await BitmapEncoder.CreateForTranscodingAsync(memStream, decoder);

                await encoder.FlushAsync();

                using (var bitmapStream = new MemoryStream())
                {
                    await memStream.AsStream().CopyToAsync(bitmapStream);
                    bitmapStream.Position = 0;
                    
                    var wImg = new WriteableBitmap(1280, 720);
                    wImg.SetSource(bitmapStream.AsRandomAccessStream());
                    try
                    {
                        var result = visionService.CalculateTranslationAndRotation(wImg);

                        System.Diagnostics.Debug.WriteLine($"Vrijednosti: x: {result.GetX()} y: {result.GetY()} z: {result.GetZ()}");
                    }
                    catch
                    {
                        System.Diagnostics.Debug.WriteLine("Aleksandar");
                    }
                }
            }

            isCaptureDone = true;
        }

        //private async Task InitializeCameraAsync()
        //{

        //    if (_mediaCapture == null)
        //    {

        //        // Get available devices for capturing pictures
        //        var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);

        //        // Get the desired camera by panel
        //        DeviceInformation cameraDevice =
        //            allVideoDevices.FirstOrDefault(x => x.EnclosureLocation != null &&
        //            x.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Back);

        //        // If there is no camera on the specified panel, get any camera
        //        cameraDevice = cameraDevice ?? allVideoDevices.FirstOrDefault();

        //        if (cameraDevice == null)
        //        {
        //            return;
        //        }

        //        // Create MediaCapture and its settings
        //        _mediaCapture = new MediaCapture();

        //        // Register for a notification when video recording has reached the maximum time and when something goes wrong
        //       // _mediaCapture.RecordLimitationExceeded += MediaCapture_RecordLimitationExceeded;

        //        var mediaInitSettings = new MediaCaptureInitializationSettings { VideoDeviceId = cameraDevice.Id };

        //        // Initialize MediaCapture
        //        try
        //        {
        //            await _mediaCapture.InitializeAsync(mediaInitSettings);
        //            _isInitialized = true;
        //        }
        //        catch (UnauthorizedAccessException)
        //        {
        //            //ShowMessageToUser("The app was denied access to the camera");
        //        }
        //        catch (Exception ex)
        //        {
        //            //Debug.WriteLine("Exception when initializing MediaCapture with {0}: {1}", cameraDevice.Id, ex.ToString());
        //        }

        //        // If initialization succeeded, start the preview
        //        if (_isInitialized)
        //        {
        //            // Figure out where the camera is located
        //            if (cameraDevice.EnclosureLocation == null || cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown)
        //            {
        //                // No information on the location of the camera, assume it's an external camera, not integrated on the device
        //                _externalCamera = true;
        //            }
        //            else
        //            {
        //                // Camera is fixed on the device
        //                _externalCamera = false;

        //                // Only mirror the preview if the camera is on the front panel
        //                _mirroringPreview = (cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Front);
        //            }

        //            await StartPreviewAsync();

        //            UpdateCaptureControls();
        //        }
        //    }
        //}
    }
}
