
// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold18pt7b.h>

// ESP32 CS(SS)=5,SCL(SCK)=18,SDA(MOSI)=23,BUSY=15,RES(RST)=2,DC=0

// 1.54'' EPD Module
//GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=5*/ 15, /*DC=*/ 4, /*RES=*/ 5, /*BUSY=*/ 16)); // GDEH0154D67 200x200, SSD1681

// 2.13'' EPD Module
//GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ 15, /*DC=*/ 4, /*RES=*/ 5, /*BUSY=*/ 16)); // DEPG0213BN 122x250, SSD1680
//GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(/*CS=5*/ 15, /*DC=*/ 4, /*RES=*/ 5, /*BUSY=*/ 16)); // GDEY0213Z98 122x250, SSD1680

// 2.9'' EPD Module
//GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(/*CS=5*/ 15, /*DC=*/ 4, /*RES=*/ 5, /*BUSY=*/ 16)); // DEPG0290BS 128x296, SSD1680
//GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(/*CS=5*/ 15, /*DC=*/ 4, /*RES=*/ 5, /*BUSY=*/ 16)); // GDEM029C90 128x296, SSD1680

// 4.2'' EPD Module
//GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=5*/ 15, /*DC=*/ 4, /*RES=*/ 5, /*BUSY=*/ 16)); // 400x300, SSD1683

// 4.26'' EPD Module
GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT> display(GxEPD2_426_GDEQ0426T82(/*CS=5*/ 5, /*DC=*/ 0, /*RES=*/ 2, /*BUSY=*/ 15)); // DEPG0213BN 122x250, SSD1680


void setup()
{
  display.init(115200,true,200,false);           //115200: Kecepatan komunikasi serial (baud rate),true: Menyalakan layar saat diinisialisasi, 50: Timeout dalam milidetik, false: Tidak mereset layar secara penuh saat inisialisasi
  helloWorld();                                 //memanggil prosedur
  helloFullScreenPartialMode();                 //memanggil prosedur
  delay(1000);
  if (display.epd2.hasFastPartialUpdate)        //Mengecek apakah layar mendukung partial update cepat
  {
    showPartialUpdate();                        //Memanggil prosedur showPartialUpdate, yang kemungkinan menampilkan pembaruan pada area tertentu layar
    delay(1000);
  }
  display.hibernate();                          //Mode ini mematikan layar untuk menghemat daya, sangat penting pada perangkat bertenaga baterai.
}

const char HelloWorld[] = "Hello World!";
const char HelloWeACtStudio[] = "Yaser Ali Husen";

void helloWorld()
{
  display.setRotation(0);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby;                             //integer dengan negatif, untuk kordinat posisi x,y
  uint16_t tbw, tbh;                            //integer non negatf, untuk lebar kotak dan tinggi kotak
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh); //Menghitung dimensi teks (lebar dan tinggi) serta offset (tbx dan tby).
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();                      //mengatur area tampilan penuh sebagai wilayah aktif untuk menggambar
  display.firstPage();                          //Memulai proses menggambar pada layar e-paper.
  do                                            //Melakukan looping untuk menggambar konten pada layar
  {
    display.fillScreen(GxEPD_WHITE);            //Membersihkan layar dan mengisinya dengan warna putih sebagai latar belakang.
    display.setCursor(x, y-tbh);                //Menentukan posisi untuk mencetak teks pertama (HelloWorld). Posisi disesuaikan agar teks sedikit lebih tinggi dari pusat (y - tbh)
    display.print(HelloWorld);                  //Menampilkan teks "Hello World!" pada koordinat yang telah dihitung sebelumnya
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK); //Mengatur warna teks berdasarkan kemampuan layar, Jika layar mendukung warna, gunakan warna merah (GxEPD_RED), Jika tidak, gunakan warna hitam (GxEPD_BLACK).
    display.getTextBounds(HelloWeACtStudio, 0, 0, &tbx, &tby, &tbw, &tbh); //Menghitung dimensi teks "WeAct Studio" (tbw, tbh) dan posisi offset (tbx, tby).
    x = ((display.width() - tbw) / 2) - tbx;    //Menghitung ulang koordinat x untuk memusatkan teks "WeAct Studio". 
    display.setCursor(x, y+tbh);                //Menentukan posisi untuk mencetak teks kedua di bawah teks pertama (y + tbh).
    display.print(HelloWeACtStudio);            //Menampilkan teks "WeAct Studio" pada layar.
  }
  while (display.nextPage());                   //Melanjutkan proses menggambar hingga semua halaman selesai ditampilkan.
}

void helloFullScreenPartialMode()
{
  //Serial.println("helloFullScreenPartialMode");
  const char fullscreen[] = "full screen update";
  const char fpm[] = "fast partial mode";
  const char spm[] = "slow partial mode";
  const char npm[] = "no partial mode";
  display.setPartialWindow(0, 0, display.width(), display.height());  //Menyetel mode parsial dengan menggunakan seluruh area layar.
  display.setRotation(0);
  display.setFont(&FreeMonoBold18pt7b);
  if (display.epd2.WIDTH < 104) display.setFont(0); //Menggunakan font FreeMonoBold9pt7b untuk teks. Jika layar terlalu kecil, font default digunakan.
  display.setTextColor(GxEPD_BLACK);
  const char* updatemode;
  if (display.epd2.hasFastPartialUpdate)        //Mengecek kemampuan layar untuk mendukung partial update
  {
    updatemode = fpm;                           //Fast partial mode (fpm): Pembaruan sebagian dengan kecepatan tinggi.
  }
  else if (display.epd2.hasPartialUpdate)
  {
    updatemode = spm;                           //Slow partial mode (spm): Pembaruan sebagian tetapi lebih lambat.
  }
  else
  {
    updatemode = npm;                           //No partial mode (npm): Tidak mendukung pembaruan parsial
  }
  // do this outside of the loop
  int16_t tbx, tby; uint16_t tbw, tbh;
  // center update text
  display.getTextBounds(fullscreen, 0, 0, &tbx, &tby, &tbw, &tbh);  //Posisi teks dihitung untuk memusatkannya di layar.
  uint16_t utx = ((display.width() - tbw) / 2) - tbx;
  uint16_t uty = ((display.height() / 4) - tbh / 2) - tby;
  // center update mode
  display.getTextBounds(updatemode, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t umx = ((display.width() - tbw) / 2) - tbx;
  uint16_t umy = ((display.height() * 3 / 4) - tbh / 2) - tby;
  // center HelloWorld
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t hwx = ((display.width() - tbw) / 2) - tbx;
  uint16_t hwy = ((display.height() - tbh) / 2) - tby;
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);            //Mengisi layar dengan warna putih (membersihkan layar)
    display.setCursor(hwx, hwy);                //Menentukan posisi kursor untuk setiap teks dan mencetaknya pada layar.
    display.print(HelloWorld);                  //"Hello World" di tengah layar.
    display.setCursor(utx, uty);              
    display.print(fullscreen);                  //"full screen update" di bagian atas tengah layar.
    display.setCursor(umx, umy);
    display.print(updatemode);                  //Mode pembaruan (seperti "fast partial mode") di bagian bawah tengah layar.
  }
  while (display.nextPage());
  Serial.println("helloFullScreenPartialMode done");
}

//Fungsi showPartialUpdate bertujuan untuk mendemonstrasikan kemampuan layar e-paper untuk melakukan pembaruan parsial, 
//yaitu menggambar atau memperbarui hanya bagian tertentu dari layar (bukan keseluruhan)
void showPartialUpdate()
{
  // some useful background
  helloWorld();
  delay(5000);
  // use asymmetric values for test
  uint16_t box_x = 10;                          //Mendefinisikan posisi (x, y), lebar, dan tinggi dari kotak pembaruan parsial di laya                    
  uint16_t box_y = 15;
  uint16_t box_w = 140;
  uint16_t box_h = 40;
  uint16_t cursor_y = box_y + box_h - 6;
  if (display.epd2.WIDTH < 104) cursor_y = box_y + 6;
  float value = 13.95;                                        //value awal
  uint16_t incr = display.epd2.hasFastPartialUpdate ? 1 : 3;  //Mengatur jumlah langkah perubahan berdasarkan kemampuan layar
  display.setFont(&FreeMonoBold18pt7b);
  if (display.epd2.WIDTH < 104) display.setFont(0);           //Argumen 0 kemungkinan merujuk pada font default atau ukuran font yang lebih kecil
  display.setTextColor(GxEPD_BLACK);
  // show where the update box is
  for (uint16_t r = 0; r < 4; r++)                            //Menjalankan blok kode di dalam for sebanyak 4 kali, dengan nilai r berturut-turut menjadi 0, 1, 2, dan 3.
  {
    display.setRotation(r);
    display.setPartialWindow(box_x, box_y, box_w, box_h);     //memperbarui hanya sebagian dari layar e-paper
    display.firstPage();
    do
    {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);  //Menggambar sebuah kotak berwarna solid yang dimulai dari koordinat (box_x, box_y) dengan lebar box_w dan tinggi box_h.
      //display.fillScreen(GxEPD_BLACK);
    }
    while (display.nextPage());
    delay(2000);
    display.firstPage();
    do
    {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    }
    while (display.nextPage());
    delay(1000);
  }
  //return;
  // show updates in the update box
  for (uint16_t r = 0; r < 4; r++)
  {
    display.setRotation(r);
    display.setPartialWindow(box_x, box_y, box_w, box_h);
    for (uint16_t i = 1; i <= 10; i += incr)
    {
      display.firstPage();
      do
      {
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);  //Mengisi area kotak dengan warna tertentu.
        display.setCursor(box_x, cursor_y);
        display.print(value * i, 2);
      }
      while (display.nextPage());
      delay(500);
    }
    delay(1000);
    display.firstPage();
    do
    {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);  //Mengisi area kotak dengan warna tertentu.
    }
    while (display.nextPage());
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
