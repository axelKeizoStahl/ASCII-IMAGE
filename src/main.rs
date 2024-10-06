use nokhwa::utils::{CameraIndex, RequestedFormat, RequestedFormatType};
use nokhwa::pixel_format::RgbFormat;
use nokhwa::Camera;
fn main() {
    // first camera in system
    let index = CameraIndex::Index(0); 
    // request the absolute highest resolution CameraFormat that can be decoded to RGB.
    let requested = RequestedFormat::new::<RgbFormat>(RequestedFormatType::AbsoluteHighestFrameRate);
    // make the camera
    let mut camera = Camera::new(index, requested).unwrap();
    let _ = camera.open_stream();
    // get a frame
    let frame = camera.frame().unwrap();
    println!("Captured Single Frame of {}", frame.buffer().len());
    // decode into an ImageBuffer
    let decoded = frame.decode_image::<RgbFormat>().unwrap();
    println!("Decoded Frame of {}", decoded.len());
}
