## About This Code 

These files contain the source code for the **UpdateMate tablet**, as featured in the exposed images. The current version supports several core features, including:

- 📅 Google Calendar event display  
- 🌦️ Weather updates  
- 📰 News headlines  
- 📈 Stock price monitoring  
- 📖 Basic eBook reading functionality

Feel free to use the code as-is, customize it for your needs, or improve it.

---

## Before You Flash the Firmware

Please review the following setup requirements:

1. **Partition Scheme**  
   Use the **2MB app / 2MB SPIFFS** partition scheme in your Arduino IDE alonside **ESP32C6 Dev Module**

2. **Uploading Icons**  
   Icon files must be uploaded via the SPIFFS filesystem. This requires an older version of the Arduino IDE.  
   👉 Follow this guide: [Install ESP32 Filesystem Uploader](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/)

3. **eBook Support**  
   Only `.txt` books are supported in the current version!  
   📚 Visit [Project Gutenberg](https://www.gutenberg.org/) to download free public domain books, then use the provided Python script to convert and upload them.

4. **Google Calendar Integration**  
   You’ll need a custom **Google Apps Script** to expose a private URL that serves your calendar data.  
   ✍️ A template script is included in the repository.

5. **API Keys Required**  
   The following services are used for fetching dynamic data:
   - [Visual Crossing](https://www.visualcrossing.com/) – for weather
   - [Twelve Data](https://twelvedata.com/) – for stock prices
   - [GNews](https://gnews.io/) – for news feed

   🗝️ Sign up for these services to obtain the necessary API keys and insert them into the configuration file.

6. **Enjoy tinkering!**