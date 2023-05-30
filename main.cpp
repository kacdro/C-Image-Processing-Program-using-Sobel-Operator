#include <stdint.h> 
#include <vector>   
#include <iostream>
#include <fstream>  

using namespace std;

uint32_t tryb_pracy = 0;

ifstream bmp_plik;


struct Piksel {
	float kolor[3] = { 0,0,0 };
};

Piksel* dane_obrazu;

struct Filtr {
	float* tablica;


	int32_t szerokosc = 0;
	int32_t wysokosc = 0;


	Filtr(int32_t szerokosc, int32_t wysokosc)
	{

		this->szerokosc = szerokosc;
		this->wysokosc = wysokosc;
	}
};

#pragma pack(1) 
struct BITMAPFILEHEADER {

	uint16_t bfType{ 0 };


	uint32_t bfSize{ 0 };


	uint16_t bfReserved1{ 0 };


	uint16_t bfReserved2{ 0 };

	uint32_t bfOffBits{ 0 };
};


struct BITMAPINFOHEADER {

	uint32_t biSize{ 0 };


	uint32_t biWidth{ 0 };


	uint32_t biHeight{ 0 };


	uint16_t biPlanes{ 0 };


	uint16_t biBitCount{ 0 };


	uint32_t biCompression{ 0 };


	uint32_t biSizeImage{ 0 };

	uint32_t biXpelsPerMeter{ 0 };


	uint32_t biYpelsPerMeter{ 0 };

	uint32_t biCrlUses{ 0 };


	uint32_t biCrlImportant{ 0 };
};
#pragma pack() 


BITMAPFILEHEADER bfh;


BITMAPINFOHEADER bih;


void wypisz_naglowki_na_konsoli()
{
	cout << "Sygnatura pliku:" << bfh.bfType << endl;
	cout << "Dlugosc calego pliku w bajtach:" << bfh.bfSize << endl;
	cout << "Pole zarezerwowane (zwykle zero) 1: " << bfh.bfReserved1 << endl;
	cout << "Pole zarezerwowane (zwykle zero) 2: " << bfh.bfReserved2 << endl;
	cout << "Pozycja danych obrazowych w pliku: " << bfh.bfOffBits << endl;
	cout << "Rozmiar naglowka informacyjnego: " << bih.biSize << endl;
	cout << "Szerokosc obrazu w pikselach: " << bih.biWidth << endl;
	cout << "Wysokosc obrazu w pikselach: " << bih.biHeight << endl;
	cout << "Liczba platow (musi byc 1): " << bih.biPlanes << endl;
	cout << "Liczba bitow na piksel: " << bih.biBitCount << endl;
	cout << "Algorytm kompresji: " << bih.biCompression << endl;
	cout << "Rozmiar rysunku: " << bih.biSizeImage << endl;
	cout << "Rozdzielczosc pozioma: " << bih.biXpelsPerMeter << endl;
	cout << "Rozdzielczosc pionowa: " << bih.biYpelsPerMeter << endl;
	cout << "Liczba kolorow w palecie: " << bih.biCrlUses << endl;
	cout << "Liczba waznych kolorow w palecie: " << bih.biCrlImportant << endl << endl;
}


int operator_sobel[8][9] = {

	 { -1, 0, 1,  -2, 0, 2,  -1, 0, 1 },

	 {  0, 1, 2,  -1, 0, 1,  -2,-1, 0 },

	 {  1, 2, 1,   0, 0, 0,  -1,-2,-1 },

	 {  2, 1, 0,   1, 0,-1,   0,-1,-2 },

	 {  1, 0,-1,   2, 0,-2,   1, 0,-1 },

	 {  0,-1,-2,   1, 0,-1,   2, 1, 0 },


	 { -1,-2,-1,   0, 0, 0,   1, 2, 1 },


	 { -2,-1, 0,  -1, 0, 1,   0, 1, 2 }
};


void odczytaj_bfh_bih(BITMAPFILEHEADER& bfh, BITMAPINFOHEADER& bih)
{



	bmp_plik.read(reinterpret_cast<char*>(&bfh.bfType), 2);


	bmp_plik.read(reinterpret_cast<char*>(&bfh.bfSize), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bfh.bfReserved1), 2);

	bmp_plik.read(reinterpret_cast<char*>(&bfh.bfReserved2), 2);


	bmp_plik.read(reinterpret_cast<char*>(&bfh.bfOffBits), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biSize), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biWidth), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biHeight), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biPlanes), 2);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biBitCount), 2);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biCompression), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biSizeImage), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biXpelsPerMeter), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biYpelsPerMeter), 4);

	bmp_plik.read(reinterpret_cast<char*>(&bih.biCrlUses), 4);


	bmp_plik.read(reinterpret_cast<char*>(&bih.biCrlImportant), 4);
}



void wczytaj_filtry(vector<Filtr>& kontener_na_filtry, string nazwa_pliku)
{
	ifstream f;
	f.open(nazwa_pliku);

	int32_t szer = 0;
	int32_t wys = 0;


	while (f >> szer >> wys)
	{

		if (szer == 0 || wys == 0 || szer % 2 == 0 || wys % 2 == 0)
		{
			cout << "Filtr jest niepoprawnie zapisany w pliku" << endl;
			exit(0);
		}

		Filtr filtr(szer, wys);


		filtr.tablica = new float[szer * wys];

		float wartosc = 0;

		int idx = 0;


		for (int i = 0; i < szer * wys; i++)
		{

			f >> wartosc;


			filtr.tablica[idx] = static_cast<float>(wartosc);


			idx = idx + 1;
		}


		kontener_na_filtry.push_back(filtr);
	}


	f.close();
}

void tablica_dynamiczna()
{
	int32_t liczba_px_wiersza = bih.biWidth;


	uint32_t wyrownanie = ((4 - (bih.biWidth * 3) % 4) % 4);


	uint8_t bajty_dopelniajace[3];

	uint8_t kolory[3] = { 0,0,0 };


	for (uint32_t wys = 0; wys < bih.biHeight; wys++)
	{
		for (uint32_t szer = 0; szer < bih.biWidth; szer++)
		{

			bmp_plik.read(reinterpret_cast<char*>(&kolory), 3);


			for (int z = 0; z < 3; z++)
				dane_obrazu[wys * bih.biWidth + szer].kolor[z] = static_cast<float>(kolory[z]);
		}


		bmp_plik.read(reinterpret_cast<char*>(&bajty_dopelniajace), wyrownanie);
	}
}

template <class T>
Piksel* konwolucja(T tablicaFiltra, int32_t rozmiarFiltraSzer, int32_t rozmiarFiltraWys)
{

	Piksel* noweDaneObrazu = new Piksel[bih.biWidth * bih.biHeight];

	int srodekx = rozmiarFiltraSzer / 2;
	int srodeky = rozmiarFiltraWys / 2;


	for (int x = 0; x < bih.biWidth; ++x)
	{

		for (int y = 0; y < bih.biHeight; ++y)
		{

			for (int fx = 0; fx < rozmiarFiltraSzer; ++fx)
			{

				for (int fy = 0; fy < rozmiarFiltraWys; ++fy)
				{

					int rx = x + (fx - srodekx);
					int ry = y + (fy - srodeky);


					if (rx >= 0 && rx < bih.biWidth && ry >= 0 && ry < bih.biHeight)
					{

						if (tryb_pracy == 0)
						{

							for (int z = 0; z < 3; z++)
								noweDaneObrazu[y * bih.biWidth + x].kolor[z] += dane_obrazu[ry * bih.biWidth + rx].kolor[z] * (tablicaFiltra[fy * rozmiarFiltraSzer + fx]);
						}

						else if (tryb_pracy == 1)
						{

							uint32_t wyrownanie = ((4 - (bih.biWidth * 3) % 4) % 4);


							uint32_t liczba_px_wiersza = bih.biWidth * 3 + wyrownanie;


							uint8_t kolor[3];

							bmp_plik.seekg(bfh.bfOffBits + liczba_px_wiersza * ry + 3 * rx);


							bmp_plik.read(reinterpret_cast<char*>(&kolor), 3);

							for (int z = 0; z < 3; z++)
								noweDaneObrazu[y * bih.biWidth + x].kolor[z] += kolor[z] * (tablicaFiltra[fy * rozmiarFiltraSzer + fx]);
						}
					}
				}
			}


			for (int z = 0; z < 3; z++)
			{
				if (noweDaneObrazu[y * bih.biWidth + x].kolor[z] < 0)
					noweDaneObrazu[y * bih.biWidth + x].kolor[z] = 0;
				else if (noweDaneObrazu[y * bih.biWidth + x].kolor[z] > 255)
					noweDaneObrazu[y * bih.biWidth + x].kolor[z] = 255;
			}
		}
	}


	return noweDaneObrazu;
}

void zapisz_format_bmp(string nazwa, Piksel* dane_obrazu)
{

	ofstream strumienZapisu;

	strumienZapisu.open(nazwa, ios::binary);

	uint8_t bajty_dopelniajace[3];

	bajty_dopelniajace[0] = 0;
	bajty_dopelniajace[1] = 0;
	bajty_dopelniajace[2] = 0;

	uint8_t wyrownanie = ((4 - (bih.biWidth * 3) % 4) % 4);

	strumienZapisu.write(reinterpret_cast<char*>(&bfh), sizeof(bfh));
	strumienZapisu.write(reinterpret_cast<char*>(&bih), sizeof(bih));


	int32_t liczba_px_wiersza = bih.biWidth;

	uint8_t kolor[3];


	for (int wys = 0; wys < bih.biHeight; wys++)
	{
		for (int szer = 0; szer < bih.biWidth; szer++)
		{
			for (int z = 0; z < 3; z++)
				kolor[z] = static_cast<uint8_t>(dane_obrazu[wys * bih.biWidth + szer].kolor[z]);


			strumienZapisu.write(reinterpret_cast<char*>(kolor), 3);
		}

		strumienZapisu.write(reinterpret_cast<char*>(bajty_dopelniajace), wyrownanie);
	}

	strumienZapisu.close();
}

Piksel* Sobel()
{
	Piksel* sobelDane[8];
	Piksel* noweDaneObrazu = new Piksel[bih.biWidth * bih.biHeight];

	for (int i = 0; i < 8; i++)
	{
		sobelDane[i] = konwolucja(operator_sobel[i], 3, 3);
	}

	for (uint32_t y = 0; y < bih.biHeight; y++)
	{
		for (uint32_t x = 0; x < bih.biWidth; x++)
		{

			noweDaneObrazu[y * bih.biWidth + x] = Piksel();

			for (int i = 0; i < 8; i++)
				for (int z = 0; z < 3; z++)
					noweDaneObrazu[y * bih.biWidth + x].kolor[z] += sobelDane[i][y * bih.biWidth + x].kolor[z];

			for (int z = 0; z < 3; z++)
				noweDaneObrazu[y * bih.biWidth + x].kolor[z] /= 8.0;
		}
	}

	return noweDaneObrazu;
}

int main()
{
	uint32_t numer_instrukcji;

	uint32_t numer_filtra;

	string nazwa_pliku;

	vector<Filtr> kontener_na_filtry;

	wczytaj_filtry(kontener_na_filtry, "lista_filtrow.txt");


	while (1)
	{
		cout << "_________ MENU _________" << endl;
		cout << "1. Wykonaj odczyt naglowkow z podanego pliku." << endl;
		cout << "2. Okresl sposob pracy programu (0 - tablica alokowana dynamicznie na piksele, 1 - odczyt piksela z pliku)." << endl;
		cout << "3. Uzyj operatora Sobel i zapisz wynik do pliku bmp" << endl;
		cout << "4. Wybierz filtr z pliku" << endl;

		cin >> numer_instrukcji;

		if (numer_instrukcji == 1)
		{
			cout << "Podaj nazwe pliku z formatem bmp (.bmp): " << endl;

			cin >> nazwa_pliku;

			bmp_plik = ifstream(nazwa_pliku, ios::binary);

			cout << "Nazwa pliku zostala wprowadzona poprawnie" << endl;

			odczytaj_bfh_bih(bfh, bih);

			wypisz_naglowki_na_konsoli();
		}
		else if (numer_instrukcji == 2)
		{
			cout << "Podaj sposob pracy programu, 0 lub 1: " << endl;

			cin >> tryb_pracy;

			dane_obrazu = new Piksel[bih.biWidth * bih.biHeight];

			if (tryb_pracy == 0)
				tablica_dynamiczna();

			cout << "Tryb pracy zostal pomyslnie ustawiony" << endl << endl;
		}
		else if (numer_instrukcji == 3)
		{
			cout << "Podaj nazwe pliku obrazu ktory chcesz zapisac: " << endl;

			cin >> nazwa_pliku;

			Piksel* dane_obrazu = Sobel();

			zapisz_format_bmp(nazwa_pliku, dane_obrazu);

			delete[] dane_obrazu;

			cout << "Plik zostal zapisany poprawnie" << endl;
		}
		else if (numer_instrukcji == 4)
		{
			cout << "Wczytane filtry z pliku:" << endl;

			for (uint32_t nr = 0; nr < kontener_na_filtry.size(); nr++)
				cout << "nr filtra: " << nr << ", szerokosc: " << kontener_na_filtry[nr].szerokosc << ", wysokosc: " << kontener_na_filtry[nr].wysokosc << endl;


			cout << "Podaj nr filtra ktory chcesz wykorzystac:" << endl;

			cin >> numer_filtra;

			cout << "Wprowadz nazwe zapisywanego pliku" << endl;

			cin >> nazwa_pliku;

			Piksel* dane_obrazu = konwolucja(kontener_na_filtry[numer_filtra].tablica, kontener_na_filtry[numer_filtra].szerokosc, kontener_na_filtry[numer_filtra].wysokosc);

			zapisz_format_bmp(nazwa_pliku, dane_obrazu);

			delete[] dane_obrazu;

			cout << "Plik zostal zapisany poprawnie" << endl;
		}
	}
	return 0;