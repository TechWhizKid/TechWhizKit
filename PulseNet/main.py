import argparse
import speedtest
import sys


def test_network_speed(test_download=True, test_upload=True):
    """Test network speed and return the results."""
    try:
        st = speedtest.Speedtest() # Initialize the Speedtest object
        st.get_best_server() # Get the best server based on ping

        # Initialize results
        download_speed_mbps = None
        upload_speed_mbps = None

        # Perform the download speed test
        if test_download:
            download_speed = st.download()
            download_speed_mbps = download_speed / 1e6

        # Perform the upload speed test
        if test_upload:
            upload_speed = st.upload()
            upload_speed_mbps = upload_speed / 1e6

        return download_speed_mbps, upload_speed_mbps
    except speedtest.SpeedtestException as e:
        print(f"Speedtest failed: {e}", file=sys.stderr)
        sys.exit(1)

def main():
    """Main function to execute the network speed test based on CLI arguments."""
    # Set up the argument parser
    parser = argparse.ArgumentParser(description='PulseNet 1.0 (x64) : (c) TechWhizKid - All rights reserved.')
    parser.add_argument('-u', '--upload', action='store_true', help='Test upload speed only.')
    parser.add_argument('-d', '--download', action='store_true', help='Test download speed only.')
    parser.add_argument('-n', '--nobanner', action='store_true', help='Suppresses the banner.')

    # Parse the arguments
    args = parser.parse_args()

    # Print the banner unless the --nobanner/-n flag is provided
    if not args.nobanner:
        print("\nPulseNet 1.0 (x64) : (c) TechWhizKid - All rights reserved.")
        print("Source - \"https://github.com/TechWhizKid/TechWhizKit\"")

    # If both upload and download are specified, or neither, test both
    test_both = args.upload and args.download or not (args.upload or args.download)

    # Run the network speed test
    download_speed, upload_speed = test_network_speed(test_download=test_both or args.download, test_upload=test_both or args.upload)

    # Print the results based on the arguments
    if test_both:
        print(f"\nDownload Speed: {download_speed:.2f} Mbps")
        print(f"Upload Speed: {upload_speed:.2f} Mbps")
    elif args.download:
        print(f"\nDownload Speed: {download_speed:.2f} Mbps")
    elif args.upload:
        print(f"\nUpload Speed: {upload_speed:.2f} Mbps")


if __name__ == "__main__":
    main()
