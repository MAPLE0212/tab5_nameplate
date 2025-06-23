#include <M5GFX.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>
#include "esp_camera.h"

M5GFX display;
int currentPage = 0; // 現在のページ番号

// カメラ設定
camera_config_t config;
bool cameraInitialized = false;

// カメラ初期化関数
bool initCamera() {
  if (cameraInitialized) return true;
  
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 15;
  config.pin_d1 = 17;
  config.pin_d2 = 18;
  config.pin_d3 = 16;
  config.pin_d4 = 14;
  config.pin_d5 = 12;
  config.pin_d6 = 11;
  config.pin_d7 = 10;
  config.pin_xclk = 13;
  config.pin_pclk = 6;
  config.pin_vsync = 8;
  config.pin_href = 7;
  config.pin_sccb_sda = 4;
  config.pin_sccb_scl = 5;
  config.pin_pwdn = 9;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_VGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }
  
  cameraInitialized = true;
  Serial.println("Camera initialized successfully!");
  return true;
}

void showPage(int page) {
  display.fillScreen(BLACK);
  
  if (page == 0) {
    // ページ1: 現在の内容
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(5);
    display.setCursor(0, 0);
    display.print("Hello, Tab5!");

    // mapleを横幅いっぱいの大きさで表示
    display.setTextSize(22);
    display.setCursor(0, 43);
    display.print("maple");

    // @pdsoh_を現在の大きさで表示
    display.setTextSize(10);
    display.setCursor(0, 210);
    display.print("@pdsoh_");

    display.setTextSize(5);
    display.setCursor(0, 310);
    display.print("Kanazawa University");

    display.setTextSize(5);
    display.setCursor(0, 350);
    display.print("");

    // 画像2枚表示 (メモリ経由) - 下5ピクセル空けて横並び
    File f1 = SPIFFS.open("/logo_ITbukatu.bmp", "r");
    if (f1) {
      size_t len = f1.size();
      uint8_t* buf = (uint8_t*)malloc(len);
      if (buf) {
        f1.read(buf, len);
        display.drawBmp(buf, len, 0, 1000);
        free(buf);
      }
      f1.close();
    }

    File f2 = SPIFFS.open("/logo_ventures.bmp", "r");
    if (f2) {
      size_t len = f2.size();
      uint8_t* buf = (uint8_t*)malloc(len);
      if (buf) {
        f2.read(buf, len);
        display.drawBmp(buf, len, 200, 1035);
        free(buf);
      }
      f2.close();
    }
    
    // 3枚目を真ん中に配置
    File f3 = SPIFFS.open("/NT2025.bmp", "r");
    if (f3) {
      size_t len = f3.size();
      uint8_t* buf = (uint8_t*)malloc(len);
      if (buf) {
        f3.read(buf, len);
        display.drawBmp(buf, len, 150, 400);
        free(buf);
      }
      f3.close();
    }
    
    // タッチ説明
    display.setTextSize(2);
    display.setCursor(0, display.height() - 30);
    display.print("Left:Next Right:Prev");
    
  } else if (page == 1) {
    // ページ2: カメラページ
    display.setTextColor(TFT_GREEN, TFT_BLACK);
    display.setTextSize(3);
    display.setCursor(0, 0);
    display.print("Camera Live View");
    
    if (!cameraInitialized) {
      display.setTextColor(TFT_YELLOW, TFT_BLACK);
      display.setTextSize(2);
      display.setCursor(0, 50);
      display.print("Initializing camera...");
      
      if (initCamera()) {
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.setTextSize(2);
        display.setCursor(0, 80);
        display.print("Camera ready!");
      } else {
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.setTextSize(2);
        display.setCursor(0, 80);
        display.print("Camera init failed!");
        display.setCursor(0, 100);
        display.print("Check camera connection");
      }
    } else {
      // カメラプレビュー表示
      camera_fb_t * fb = esp_camera_fb_get();
      if (fb) {
        // プレビュー画像を表示（画面中央に配置）
        int previewWidth = 320;  // プレビュー幅
        int previewHeight = 240; // プレビュー高さ
        int x = (display.width() - previewWidth) / 2;
        int y = 100;
        
        display.drawJpg(fb->buf, fb->len, x, y, previewWidth, previewHeight);
        esp_camera_fb_return(fb);
        
        // ステータス表示
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.setTextSize(2);
        display.setCursor(0, 50);
        display.print("Live Camera Feed");
      } else {
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.setTextSize(2);
        display.setCursor(0, 50);
        display.print("Failed to get camera frame");
      }
    }
    
    // タッチ説明
    display.setTextSize(2);
    display.setCursor(0, display.height() - 30);
    display.print("Left:Next Right:Prev");
    
  } else if (page == 2) {
    // ページ3: SPIFFSファイル一覧と画像表示
    display.setTextColor(TFT_MAGENTA, TFT_BLACK);
    display.setTextSize(3);
    display.setCursor(0, 0);
    display.print("SPIFFS Files:");
    
    if (!SPIFFS.begin()) {
      display.setTextColor(TFT_RED, TFT_BLACK);
      display.setTextSize(2);
      display.setCursor(0, 50);
      display.print("SPIFFS Error!");
    } else {
      display.setTextColor(TFT_GREEN, TFT_BLACK);
      display.setTextSize(2);
      display.setCursor(0, 50);
      display.print("SPIFFS OK!");
      
      // ファイル一覧を表示
      File root = SPIFFS.open("/");
      int yPos = 80;
      int fileCount = 0;
      bool imageFound = false;
      
      while (File file = root.openNextFile()) {
        if (yPos < display.height() - 50) {
          display.setTextColor(TFT_CYAN, TFT_BLACK);
          display.setTextSize(5);
          display.setCursor(0, yPos);
          
          String fileName = file.name();
          display.print("[FILE] ");
          display.print(fileName);
          display.print(" (");
          display.print(file.size());
          display.print(" bytes)");
          
          // 画像ファイルかチェック
          if (fileName.endsWith(".jpg") || fileName.endsWith(".png") || 
              fileName.endsWith(".bmp") || fileName.endsWith(".gif")) {
            imageFound = true;
            display.setTextColor(TFT_YELLOW, TFT_BLACK);
            display.print(" [IMAGE]");
          }
          
          yPos += 15;
          fileCount++;
        }
        file.close();
      }
      root.close();
      
      if (fileCount == 0) {
        display.setTextColor(TFT_ORANGE, TFT_BLACK);
        display.setTextSize(2);
        display.setCursor(0, 80);
        display.print("No files found");
        display.setCursor(0, 100);
        display.setTextSize(1);
        display.print("Upload images to /data folder");
      } else if (imageFound) {
        // 画像表示ボタン
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.setTextSize(2);
        display.setCursor(0, display.height() - 60);
        display.print("Tap to view images");
      }
    }
    
    // タッチ説明
    display.setTextSize(2);
    display.setCursor(0, display.height() - 30);
    display.print("Left:Next Right:Prev");
  }
}

void setup()
{
  display.begin();
  display.setRotation(0);
  
  // SPIFFSの初期化
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
  } else {
    Serial.println("SPIFFS initialized successfully!");
  }
  
  showPage(currentPage);
}

void loop()
{
  // カメラページの場合はリアルタイム更新
  if (currentPage == 1 && cameraInitialized) {
    showPage(currentPage);
  }
  
  // タッチ入力の処理
  lgfx::touch_point_t tp;
  if (display.getTouch(&tp)) {
    int touchX = tp.x;
    int touchY = tp.y;
    
    // 画面の左半分をタップ: 次のページ
    if (touchX < display.width() / 2) {
      currentPage = (currentPage + 1) % 3;
      showPage(currentPage);
      delay(300); // タッチの重複を防ぐ
    }
    // 画面の右半分をタップ: 前のページ
    else {
      currentPage = (currentPage - 1 + 3) % 3;
      showPage(currentPage);
      delay(300); // タッチの重複を防ぐ
    }
  }
  
  delay(100); // カメラ更新のための遅延
}

