#include <algorithm>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <io.h>
#include <random>
#include <string>
#include <map>
#include <Windows.h>
#include <vector>

std::wifstream otworzPlik(std::wstring nazwa_pliku);
static void zamknijPlikWejsciowy(std::wifstream& plik_wejsciowy);
std::vector<std::pair<std::wstring, std::vector<std::wstring>>> odczytajKategorieOrazHasla(std::wifstream& plik_wejsciowy);
std::wofstream otworzPlikWyjsciowy();
static void zamknijPlikWyjsciowy(std::wofstream& plik_wyjsciowy);
static void dopisanieNumerowPol(std::wofstream& plik_wyjsciowy, int liczba_pol);
static void generujKartyBingo(std::vector<std::pair<std::wstring, std::vector<std::wstring>>>& kategorie_oraz_hasla, int liczba_kart_bingo, int liczba_pol, std::wofstream& plik);
std::vector<std::pair<int, std::vector<int>>> ponumerujKategorieOrazHasla(std::vector<std::pair<std::wstring, std::vector<std::wstring>>>& kategorie_oraz_hasla, int liczba_hasel);

int main() {
    SetConsoleOutputCP(65001);
    (void)_setmode(_fileno(stdout), _O_U8TEXT);
    (void)_setmode(_fileno(stdin), _O_U8TEXT);

    std::wstring nazwa_pliku_wejsciowego;
    std::wcout << L"(Pamiętaj o podaniu rozszerzenia pliku!)" << std::endl << L"Proszę podać nazwę pliku wejściowego: ";
    std::wcin >> nazwa_pliku_wejsciowego;

    std::wifstream plik_wejsciowy = otworzPlik(nazwa_pliku_wejsciowego);
    std::vector<std::pair<std::wstring, std::vector<std::wstring>>> kategorie_oraz_hasla = odczytajKategorieOrazHasla(plik_wejsciowy);
    zamknijPlikWejsciowy(plik_wejsciowy);
    
    int liczba_kart_bingo;
    std::wcout << L"Liczba kart bingo: ";
    std::wcin >> liczba_kart_bingo;

    int liczba_pol;
    std::wcout << L"Liczba pól na jednej karcie bingo: ";
    std::wcin >> liczba_pol;

    std::wofstream plik_wyjsciowy = otworzPlikWyjsciowy();
    dopisanieNumerowPol(plik_wyjsciowy, liczba_pol);

    generujKartyBingo(kategorie_oraz_hasla, liczba_kart_bingo, liczba_pol, plik_wyjsciowy);

    zamknijPlikWyjsciowy(plik_wyjsciowy);
    return 0;
}

// Definicje funkcji
std::wifstream otworzPlik(std::wstring nazwa_pliku){
    std::wifstream plik_wejsciowy(nazwa_pliku);

    if(!plik_wejsciowy.is_open()){
        std::wcout << L"Brak takiego pliku w folderze z programem.";
        exit(1);
    }

    return plik_wejsciowy;
}

static void zamknijPlikWejsciowy(std::wifstream& plik_wejsciowy){
    plik_wejsciowy.close();
}

std::vector<std::pair<std::wstring, std::vector<std::wstring>>> odczytajKategorieOrazHasla(std::wifstream& plik_wejsciowy){
    std::vector<std::pair<std::wstring, std::vector<std::wstring>>> kategorie_oraz_hasla;
    std::wstring wiersz, kategoria, haslo;
    size_t pozycjaTab, pozycjaEnter;

    while(std::getline(plik_wejsciowy, wiersz)){
        pozycjaTab = wiersz.find(L'\t');
        pozycjaEnter = wiersz.find(L'\n');
        if(pozycjaTab != std::wstring::npos){
            kategoria = wiersz.substr(0, pozycjaTab);
            if(kategorie_oraz_hasla.empty() || kategoria != kategorie_oraz_hasla.back().first){
                kategorie_oraz_hasla.emplace_back(kategoria, std::vector<std::wstring>{});
            }
            if(pozycjaEnter != std::wstring::npos){
                haslo = wiersz.substr(pozycjaTab + 1, pozycjaEnter - (pozycjaTab + 1));
            } else{
                haslo = wiersz.substr(pozycjaTab + 1);
            }
        }
        kategorie_oraz_hasla.back().second.push_back(haslo);
    }
    return kategorie_oraz_hasla;
}

std::wofstream otworzPlikWyjsciowy(){
    std::string nazwa_pliku_wyjsciowego = "KartyBingo.txt";
    std::wofstream plik_wyjsciowy(nazwa_pliku_wyjsciowego);
    if(!plik_wyjsciowy.is_open()){
        std::wcout << L"Plik wyjściowy nie utworzony?";
        exit(1);
    }
    return plik_wyjsciowy;
}

static void zamknijPlikWyjsciowy(std::wofstream& plik_wyjsciowy){
    plik_wyjsciowy.close();
    std::wcout << L"Plik utworzony.";
    std::wcin.clear();
    std::wcin.ignore();
    std::wcin.get();
}

static void dopisanieNumerowPol(std::wofstream& plik_wyjsciowy, int liczba_pol){
    std::wstring naglowek;
    std::wcout << L"Podaj nazwę nagłówka: ";
    std::wcin >> naglowek;
    for(int i = 0; i < liczba_pol; i++){
        plik_wyjsciowy << naglowek + std::to_wstring(i + 1) << (i < (liczba_pol - 1) ? "\t" : "\n");
    }
}

static void generujKartyBingo(std::vector<std::pair<std::wstring, std::vector<std::wstring>>>& kategorie_oraz_hasla, int liczba_kart_bingo, int liczba_pol, std::wofstream& plik){
    std::vector<std::pair<int, std::vector<int>>> ponumerowane_kategorie_oraz_hasla;

    int liczba_kategorii = static_cast<int>(kategorie_oraz_hasla.size());
    int liczba_hasel = 0;

    for(int liczba_kart = 0; liczba_kart < liczba_kart_bingo; ){
        for(int i = 0; i < liczba_pol; i++){
            if(ponumerowane_kategorie_oraz_hasla.size() < liczba_pol){
                ponumerowane_kategorie_oraz_hasla = ponumerujKategorieOrazHasla(kategorie_oraz_hasla, liczba_hasel);
            }
            plik << kategorie_oraz_hasla[ponumerowane_kategorie_oraz_hasla[i].first].second[ponumerowane_kategorie_oraz_hasla[i].second.back()] << (i < (liczba_pol - 1) ? "\t" : "");
            ponumerowane_kategorie_oraz_hasla[i].second.pop_back();
            if(ponumerowane_kategorie_oraz_hasla[i].second.empty()){
                ponumerowane_kategorie_oraz_hasla.erase(ponumerowane_kategorie_oraz_hasla.begin() + i);
            }
        }

        liczba_kart++;
        plik << "\n";


        if(liczba_kart == liczba_kart_bingo){
            zamknijPlikWyjsciowy(plik);
            exit(1);
        }
    }
}

std::vector<std::pair<int, std::vector<int>>> ponumerujKategorieOrazHasla(std::vector<std::pair<std::wstring, std::vector<std::wstring>>>& kategorie_oraz_hasla, int liczba_hasel){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<std::pair<int, std::vector<int>>> ponumerowane_kategorie_oraz_hasla;

    for(int licznik_kategorii = 0; licznik_kategorii < kategorie_oraz_hasla.size(); licznik_kategorii++){
        ponumerowane_kategorie_oraz_hasla.emplace_back(licznik_kategorii, std::vector<int>{});

        for(int licznik_hasel = 0; licznik_hasel < kategorie_oraz_hasla[licznik_kategorii].second.size(); licznik_hasel++){
            ponumerowane_kategorie_oraz_hasla[licznik_kategorii].second.push_back(licznik_hasel);
            liczba_hasel++;
        }
    }
    std::shuffle(ponumerowane_kategorie_oraz_hasla.begin(), ponumerowane_kategorie_oraz_hasla.end(), gen);

    for(int i = 0; i < ponumerowane_kategorie_oraz_hasla.size(); i++){
        std::shuffle(ponumerowane_kategorie_oraz_hasla[i].second.begin(), ponumerowane_kategorie_oraz_hasla[i].second.end(), gen);
    }
    return ponumerowane_kategorie_oraz_hasla;
}