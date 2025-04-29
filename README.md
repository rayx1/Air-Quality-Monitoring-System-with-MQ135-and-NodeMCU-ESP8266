# Air-Quality-Monitoring-System-with-MQ135-and-NodeMCU-ESP8266

A system that monitors air quality using MQ135 sensor, triggers a buzzer alarm when thresholds are exceeded, and sends email notifications with cooldown periods.

## Features
- Real-time air quality monitoring with MQ135 sensor
- Audible alert with active-low buzzer
- Email notifications via SMTP
- 5-minute email cooldown period
- WiFi connection resilience
- Serial monitor debugging output

## Components
| Component       | Quantity |
|-----------------|----------|
| NodeMCU ESP8266 | 1        |
| MQ135 Sensor    | 1        |
| Active-Low Buzzer | 1      |
| Breadboard      | 1        |
| Jumper Wires    | 4-5      |

## Wiring Diagram (Text-Based)
## NodeMCU ESP8266 MQ135 Sensor Active-Low Buzzer
+---------------+ +------------+ +---------------+
| | | | | |
| 3V3 |-----| VCC | | |
| | | | | |
| GND |-----| GND | | |
| | | | | |
| A0 |-----| AOUT | | |
| | | | | |
| D5 |-----------------------| + (Positive) |
| | | | | |
| GND |-----------------------| - (Negative) |
| | | | | |
+---------------+ +------------+ +---------------+

## Installation

# 1. Install required libraries:

  ESP_Mail_Client by Mobizt

  ESP8266WiFi

# 2. Update configuration in the code:

  WiFi credentials

  Email settings (SMTP server, credentials)

  Air quality threshold

## Configuration
    Modify these constants in the code:
    
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define SMTP_HOST "smtp.yourprovider.com"
#define AUTHOR_EMAIL "your_email@domain.com"
#define AUTHOR_PASSWORD "your_email_password"
#define RECIPIENT_EMAIL "recipient@domain.com"
#define AIR_QUALITY_THRESHOLD 110  // Adjust based on your environment

## Usage
Upload the sketch to your NodeMCU

Open Serial Monitor (9600 baud) to view readings

System will:

  Continuously monitor air quality

  Activate buzzer when threshold is exceeded

  Send email alerts (with 5-minute cooldown)

  Display status in Serial Monitor

##  Serial Monitor Output Example

System initialized - Active-Low Buzzer
--------------------------------------
Air Quality: 210 | Buzzer: OFF (Inactive)
Air Quality: 315 | Buzzer: ON (Active)
Preparing to send email...
Email sent successfully
Email cooldown period started

## Troubleshooting

Buzzer not sounding: Verify active-low configuration and wiring

Email failures: Check SMTP settings and WiFi connection

Inaccurate readings: Calibrate MQ135 sensor for your environment

## License
This project is licensed under the MIT License - see the LICENSE file for details.
    
