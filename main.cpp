// Bankomat pro více akcí s perzistencí dat do souboru

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>  // pro funkci sleep()

using namespace std;

const int MAX_UCTU = 100;
int pocetAktualnich = 0;
string ucty[MAX_UCTU];
string piny[MAX_UCTU];
int zustatky[MAX_UCTU];
const int delitelebankovek[6] = {5000, 2000, 1000, 500, 200, 100};

// Kontrola, zda částka je násobkem 100 (bankovky)
bool kontrolacastky(int a) {
    return a > 0 && a % 100 == 0;
}

// Načte účty ze souboru "ucty.txt"
void loadAccounts(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cerr << "Soubor " << filename << " nebyl nalezen. Vytvářím nový.\n";
        return;
    }

    string line;
    pocetAktualnich = 0;
    while (getline(in, line) && pocetAktualnich < MAX_UCTU) {
        if (line.empty()) continue;
        stringstream ss(line);
        string zust;
        if (getline(ss, ucty[pocetAktualnich], ';') &&
            getline(ss, piny[pocetAktualnich], ';') &&
            getline(ss, zust)) {
            zustatky[pocetAktualnich] = stoi(zust);
            ++pocetAktualnich;
        }
    }
}

// Uloží všechny účty do souboru "ucty.txt"
void saveAccounts(const string& filename) {
    ofstream out(filename);
    if (!out) {
        cerr << "Chyba při otevírání souboru " << filename << " pro zápis.\n";
        return;
    }
    for (int i = 0; i < pocetAktualnich; ++i) {
        out << ucty[i] << ";" << piny[i] << ";" << zustatky[i] << "\n";
    }
}

// Přihlášení uživatele, vrací jeho index nebo -1
int overeni() {
    string ucet, pin;
    cout << "Číslo účtu: ";
    cin >> ucet;
    for (int i = 0; i < pocetAktualnich; ++i) {
        if (ucty[i] == ucet) {
            cout << "PIN: ";
            cin >> pin;
            if (piny[i] == pin) {
                return i;
            } else {
                cout << "Špatný PIN.\n";
                return -1;
            }
        }
    }
    cout << "Účet nenalezen.\n";
    return -1;
}

// Vklad na účet 'uzivatel'
void vklad(int uzivatel, const string& filename) {
    int castka;
    cout << "Kolik chcete vložit? (0 pro zpět): ";
    cin >> castka;
    if (castka == 0) return;
    if (!kontrolacastky(castka)) {
        cout << "Nelze vložit mince.\n";
        return;
    }
    zustatky[uzivatel] += castka;
    cout << "Vloženo " << castka << " Kč\n";
    saveAccounts(filename);
}

// Výběr z účtu 'uzivatel' s rozpiskou bankovek
void vyber(int uzivatel, const string& filename) {
    int castka;
    cout << "Kolik chcete vybrat? (0 pro zpět): ";
    cin >> castka;
    if (castka == 0) return;
    if (castka < 0 || castka > zustatky[uzivatel] || !kontrolacastky(castka)) {
        cout << "Nelze vybrat.\n";
        return;
    }
    zustatky[uzivatel] -= castka;
    cout << "Vybráno " << castka << " Kč\nRozpis bankovek:\n";
    int zbytek = castka;
    for (int i = 0; i < 6; ++i) {
        int nomin = delitelebankovek[i];
        int pocet = zbytek / nomin;
        if (pocet > 0) {
            cout << " " << nomin << " Kč x " << pocet << "\n";
            zbytek %= nomin;
        }
    }
    saveAccounts(filename);
    sleep(5);
}

// Kontrola zůstatku na 5 sekund
void kontrolaZustatku(int uzivatel) {
    cout << "Zůstatek: " << zustatky[uzivatel] << " Kč\n";
    sleep(5);
}

int main() {
    const string filename = "ucty.txt";
    loadAccounts(filename);

    // Pokud žádné účty, vytvoříme výchozí
    if (pocetAktualnich == 0) {
        ucty[0] = "1234567890"; piny[0] = "1234"; zustatky[0] = 10000;
        ucty[1] = "1111111111"; piny[1] = "0000"; zustatky[1] = 5000;
        ucty[2] = "2222222222"; piny[2] = "2222"; zustatky[2] = 20000;
        pocetAktualnich = 3;
        saveAccounts(filename);
    }

    int uzivatel = -1;
    while (true) {
        if (uzivatel < 0) {
            uzivatel = overeni();
        } else {
            int volba;
            cout << "\n1: Vklad  2: Výběr  3: Zůstatek  4: Nový uživatel  5: Konec -> ";
            cin >> volba;
            switch (volba) {
                case 1:
                    vklad(uzivatel, filename);
                    break;
                case 2:
                    vyber(uzivatel, filename);
                    break;
                case 3:
                    kontrolaZustatku(uzivatel);
                    break;
                case 4:
                    uzivatel = -1;
                    break;
                case 5:
                    cout << "Děkujeme za použití bankomatu.\n";
                    return 0;
                default:
                    cout << "Neplatná volba.\n";
            }
        }
    }
    return 0;
}
