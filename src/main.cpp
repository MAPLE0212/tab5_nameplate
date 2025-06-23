#include <M5GFX.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>

M5GFX display;
int currentPage = 0; // 現在のページ番号

void showPage(int page) {
  display.fillScreen(BLACK);
  
  if (page == 0) {
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(5);
    display.setCursor(0, 0);
    display.print("Hello, Tab5!");

    display.setTextSize(22);
    display.setCursor(0, 43);
    display.print("name");

    display.setTextSize(10);
    display.setCursor(0, 210);
    display.print("@aaaaaaaaaaaa");

    display.setTextSize(5);
    display.setCursor(0, 310);
    display.print("");

    display.setTextSize(5);
    display.setCursor(0, 350);
    display.print("");

    File f1 = SPIFFS.open("/", "r"); // 画像ファイル名を指定
    if (f1) {
      size_t len = f1.size();
      uint8_t* buf = (uint8_t*)malloc(len);
      if (buf) {
        f1.read(buf, len);
        display.drawBmp(buf, len, 0, 1000); //配置調整
        free(buf);
      }
      f1.close();
    }

    // File f2 = SPIFFS.open("/", "r");
    // if (f2) {
    //   size_t len = f2.size();
    //   uint8_t* buf = (uint8_t*)malloc(len);
    //   if (buf) {
    //     f2.read(buf, len);
    //     display.drawBmp(buf, len, 200, 1035); //配置調整
    //     free(buf);
    //   }
    //   f2.close();
    // }
    
    // File f3 = SPIFFS.open("/", "r");
    // if (f3) {
    //   size_t len = f3.size();
    //   uint8_t* buf = (uint8_t*)malloc(len);
    //   if (buf) {
    //     f3.read(buf, len);
    //     display.drawBmp(buf, len, 150, 400); //配置調整
    //     free(buf);
    //   }
    //   f3.close();
    // }
    
    // タッチ説明
    display.setTextSize(2);
    display.setCursor(0, display.height() - 30);
    display.print("Left:Next Right:Prev");

  } else if (page == 1) {
    display.fillScreen(BLACK);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(5);
    display.setCursor(0, 0);
    display.print("Page1");
    // タッチ説明
    display.setTextSize(2);
    display.setCursor(0, display.height() - 30);
    display.print("Left:Next Right:Prev");

  } else if (page == 2) {
    // ページ3: SPIFFSファイル一覧表示に使用
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
      }
    }
    }
    // タッチ説明
    display.setTextSize(2);
    display.setCursor(0, display.height() - 30);
    display.print("Left:Next Right:Prev");
  }


void setup() {
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

void loop() {
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