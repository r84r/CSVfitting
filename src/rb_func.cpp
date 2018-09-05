/*******************************************************************************
*
*  Eigene Funktionen
*  ^^^^^^^^^^^^^^^^^
*  Stand: 21.03.2012
*
*******************************************************************************/

#include <string>		// Verwendung von String
#include <sstream>	// Verarbeitung von Strings
#include <iostream>	// Dateieingabe und -ausgabe
#include <ctime>		// Zeit
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cfloat>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//   Mathematische Funktionen
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* Symmetric arithmetic rounding, n in -15..15 */
double round(double x, int n) {
	bool neg = (x < 0.0);
	double ipart, fpart;
	double y, p;
	int id;

	if (neg) x = -x;
	if (n > 0) {
		double yy;
		fpart = modf(x,&ipart);
		if (n > DBL_DIG) n = DBL_DIG;
		p = pow(10,n);
		y = fpart * p;
		fpart = modf(y,&yy);
		if (fpart < 0.5) yy += 1.0;
		y = ipart + yy / p;
	}	else if (n < 0) {
		if (n < -DBL_DIG)
		n = -DBL_DIG;
		p = pow(10, -n);
		y = x / p;
		y = round(y,0) * p;
	}	else { /* n == 0 */
		fpart = modf(x,&ipart);
		y = (fpart < 0.5)? ipart: ipart + 1;
	}
	return neg?-y:y;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//   Umwandlungsfunktionen
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

string IntToString(int val) {
	ostringstream outStream;
	outStream << val;
	return outStream.str();
}
int StringToInt(string val) {
	int i;
	istringstream inStream(val);
	inStream >> i;
	return i;
}
string TimeToSQLString(time_t val) {
	struct tm *ptm;
	char   buffer[80];
	ptm = gmtime(&val);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", ptm);
	return buffer;
}
time_t SQLStringToTime(string val) {
	struct tm dt;
	dt.tm_year = StringToInt(val.substr(0,4));
	dt.tm_mon  = StringToInt(val.substr(5,2));
	dt.tm_mday = StringToInt(val.substr(8,2));
	dt.tm_hour = StringToInt(val.substr(11,2));
	dt.tm_min  = StringToInt(val.substr(14,2));
	dt.tm_sec  = StringToInt(val.substr(17,2));
	return mktime(&dt);
}
char* StringToChars(string val) {
	char* res = new char[256];
	strcpy(res, val.c_str());
	return res;
}
string DoubleToString(double value, const int precision = -1) {
	int n;
	char buffer[64];
	char format[16];
	std::stringstream ss;

	if (precision<0) {
		ss << "%f";
	} else {
		ss << "%0." << precision << "f";
	}
	ss >> format;
	n = sprintf(buffer, format, value);
	return buffer;
}
double StringToDouble(string value, const int precision = -1) {
	double d;
	int pos;

	pos = value.find(",");
	if (pos!=string::npos) {value.replace(pos,1,".");}

	if (precision<0) {
		d = atof(value.c_str());
	} else {
		pos = value.find(".");
		if (pos!=string::npos) {
			value = value.substr(0, pos+1+precision);
			d = atof(value.c_str());
		} else {
			d = atof(value.c_str());
		}
	}
	return d;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//   Hilfsfunktionen
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool file_exists(const string& filename) {
  ifstream file(filename.c_str());
  bool result = file.good();
  file.close();
  return result;
}
string ExtractFilenamePart(const string FileName, const bool Path, const bool Name, const bool Extension) {
	// extrahiert die angegebenen Teile eines Dateinamens
	string spath = "", sname = "", sext = "";
	int ibackslash	= FileName.find_last_of("\\");
	int ipoint 			= FileName.find_last_of(".");

	if (ibackslash!=string::npos) {
		spath = FileName.substr(0,ibackslash+1);
		if (ipoint!=string::npos) {
			sname	= FileName.substr(ibackslash+1,ipoint-ibackslash-1);
			sext	= FileName.substr(ipoint+1);
		} else {
			sname	= FileName.substr(ibackslash+1);
		}
	} else {
		if (ipoint!=string::npos) {
			sname	= FileName.substr(0,ipoint);
			sext	= FileName.substr(ipoint+1);
		} else {
			sname = FileName;
		}
	}

	if (!Path)			spath = "";
	if (!Name)			sname = "";
	if (!Extension)	sext  = "";
	if (Name and Extension) sext = "." + sext;

	return spath+sname+sext;
}
int CountSubstr(string Str, string Chars) {
	int pos = 0;
	int count = 0;
	do {
		pos = Str.find(Chars, pos);
		if (pos!=string::npos) {
			count++;
			pos += Chars.length();
		}
	} while (pos!=string::npos);
	return count;
}
string RemoveQuotes(string Str) {
	if (Str.substr(0,1)=="\"" and Str.substr(Str.length()-1,1)=="\"") {
		return Str.substr(1,Str.length()-2);
	} else {
		return Str;
	}
}
string AddQuotes(string Str) {
	return "\"" + Str + "\"";
}
string BoolToString(const bool val, const string true_str, const string false_str) {
	if (val==true) {
		return true_str;
	} else {
		return false_str;
	}
}
void SplitCSVLine(string CSVLine, char Separator, vector<string>& Tokens) {
	// Trennt eine Zeile anhand eines(!) Zeichens
	string elem = "";
	string temp;
	istringstream sline(CSVLine);
	Tokens.clear();

	while (true) {
		temp = "";
		// Text bis zum ersten Komma einlesen
		if (getline(sline,temp,Separator)) {
			elem += temp;
		} else {
			// Zeilenende -> letztes Element in Vector schreiben
			if(!elem.empty()) Tokens.push_back(RemoveQuotes(elem));
			return;
		}
		// Prüfen ob Anführungszeichen (") im Element ist
		if ((CountSubstr(elem,"\"") % 2)==0) {
			// gerade Anzahl an Anführungszeichen -> Element komplett
			Tokens.push_back(RemoveQuotes(elem));
			// Elementstring zurücksetzen
			elem = "";
		} else {
			// ungerade Anzahl -> Komma Teil der Zeichenkette -> weiter in der Schleife
			elem += Separator;
		}
	}
}
void SplitString(string Str, string Separator, vector<string>& Tokens) {
	// Trennt einen String einhand einer Zeichenfolge (auch mehr wie 1 Zeichen)
	// Der String kann sich auch über mehre Zeilen erstrecken
	// Anführungszeichen werden mit beachtet
	istringstream sline(Str);
	Tokens.clear();
	string elem = "";
	int seplen = Separator.length();
	if (Separator.substr(0,1)=="\\") {
		if (Separator=="\\t") {
			seplen = 1; Separator="\t";
		} else {
			cout << endl << "Achtung: SplitString(..) verwendet ein unbekanntes Escape-Zeichen" << endl;
		}
	}
	int strlen = Str.length();
	int pos = 0;
	int lastpos = -seplen;

	while (pos+seplen<=strlen) {
		// Text bis zum ersten Separator einlesen
		if (Str.substr(pos,seplen)==Separator) {
			elem = Str.substr(lastpos+seplen,pos-lastpos-seplen);
			if (elem.substr(0,1)=="\"") {
				// Element mit Anführungszeichen -> nur hinzufügen wenn auch Anführungszeichen hinten
				if (elem.substr(elem.length()-1)=="\"") {
					Tokens.push_back(RemoveQuotes(elem));
					elem = "";
					lastpos = pos;
				} else {
					pos++;
				}
			} else {
				// Element hinzufügen
				Tokens.push_back(RemoveQuotes(elem));
				elem = "";
				lastpos = pos;
			}
			pos += seplen;
		} else {
			pos++;
		}
	}
	elem = Str.substr(lastpos+seplen);
	Tokens.push_back(RemoveQuotes(elem));
}
string& TrimLeft(string& str, const char* delimiters = " \t\n\r\f\v") {
	str.erase(0, str.find_first_not_of(delimiters));
	return str;
}
string& TrimRight(string& str, const char* delimiters = " \t\n\r\f\v") {
	str.erase(str.find_last_not_of(delimiters) + 1);
	return str;
}
string& Trim(string& str, const char* delimiters = " \t\n\r\f\v") {
	str.erase(0, str.find_first_not_of(delimiters));
	str.erase(str.find_last_not_of(delimiters) + 1);
	return str;
}
