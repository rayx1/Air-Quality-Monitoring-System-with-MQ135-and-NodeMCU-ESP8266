#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "WIFI"
#define WIFI_PASSWORD "12345678"

// Email configuration
#define SMTP_HOST "smtp.yourprovider.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "your_email@domain.com"
#define AUTHOR_PASSWORD "your_email_password"
#define RECIPIENT_EMAIL "recipient@domain.com"

// Sensor and buzzer pins
#define MQ135_PIN A0
#define BUZZER_PIN D5

// Threshold values
#define AIR_QUALITY_THRESHOLD 110  // Adjust based on your sensor calibration
#define BUZZER_INTERVAL 1000       // 1 second beep interval
#define EMAIL_COOLDOWN 300000      // 5 minutes in milliseconds (5 * 60 * 1000)

SMTPSession smtp;
unsigned long lastEmailTime = 0;
bool emailOnCooldown = false;
unsigned long lastBuzzerTime = 0;
bool buzzerState = HIGH; // Start with buzzer OFF (active-low)
bool buzzerActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); // Initialize buzzer OFF (active-low)
  
  connectToWiFi();
  
  // Setup email (won't actually connect until needed)
  smtp.debug(1);
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";
  
  smtp.connect(&session);

  Serial.println("\nSystem initialized - Active-Low Buzzer");
  Serial.println("--------------------------------------");
}

void loop() {
  int airQuality = analogRead(MQ135_PIN);
  Serial.print("Air Quality: ");
  Serial.print(airQuality);
  Serial.print(" | Buzzer: ");
  Serial.println(buzzerActive ? "ON (Active)" : "OFF (Inactive)");
  
  // Handle email cooldown
  if (emailOnCooldown && (millis() - lastEmailTime >= EMAIL_COOLDOWN)) {
    emailOnCooldown = false;
    Serial.println("Email cooldown period ended");
  }
  
  if (airQuality > AIR_QUALITY_THRESHOLD) {
    // Activate buzzer alarm (non-blocking, active-low logic)
    unsigned long currentMillis = millis();
    if (currentMillis - lastBuzzerTime >= BUZZER_INTERVAL) {
      lastBuzzerTime = currentMillis;
      buzzerState = !buzzerState; // Toggle state
      digitalWrite(BUZZER_PIN, buzzerState);
      buzzerActive = (buzzerState == LOW); // Active when LOW
    }
    
    // Try to send email if not on cooldown and WiFi is available
    if (!emailOnCooldown && WiFi.status() == WL_CONNECTED) {
      if (sendEmailAlert(airQuality)) {
        lastEmailTime = millis();
        emailOnCooldown = true;
        Serial.println("Email sent, cooldown period started");
      }
    }
  } else {
    // Air quality is good - turn off buzzer (set HIGH for active-low)
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerActive = false;
    
    // Reset email cooldown only when air quality returns to normal
    emailOnCooldown = false;
  }
  
  // Handle WiFi reconnection if needed
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  
  delay(500);  // Short delay between readings
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Continuing without network.");
  }
}

bool sendEmailAlert(int airQualityValue) {
  Serial.println("Preparing to send email...");
  
  SMTP_Message message;
  message.sender.name = "Air Quality Monitor";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "AIR QUALITY ALERT!";
  message.addRecipient("Recipient", RECIPIENT_EMAIL);
  
  String textMsg = "Warning! Poor air quality detected.\n";
  textMsg += "Current reading: " + String(airQualityValue) + "\n";
  textMsg += "Threshold: " + String(AIR_QUALITY_THRESHOLD) + "\n\n";
  textMsg += "Time: " + String(millis() / 1000) + " seconds since startup";
  
  message.text.content = textMsg.c_str();
  
  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email, " + smtp.errorReason());
    return false;  // Email failed, don't start cooldown
  }
  
  Serial.println("Email sent successfully");
  return true;  // Email succeeded, start cooldown
}