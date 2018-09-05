/*******************************************************************************
*
*  CSVfitting - by Ralf Bartsch
*  ============================
*  Verarbeitet eine CSV-Datei aus Messwerten durch Zusammenfassen von einer
*  festgelegten Anzahl von Zeilen oder Umwandeln jeder einzeln Zeile. Dabei
*  kann für jede Spalte angegeben werden wie diese bearbeitet werden soll.
*
*  Versionshistorie:
*  -----------------
*  1.0.0 : 21.03.2012
*    - erste Version
*    - ToDo: Datum/Zeit-Verarbeitung
*
*
*******************************************************************************/

#include <iostream>	// Dateiein- und -ausgaben
#include <string>		// Verwendung von String
#include <ctime>		// Zeit
#include <sstream>	// Verarbeitung von Strings
#include <fstream>	// Dateioperationen
#include <vector>		// Verwendung von Vektoren
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "rb_func.cpp"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  Programmspezifische Funktionen
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const int ERR_CONFIG_NOT_FOUND	= 1;
const int ERR_INPUT_NOT_FOUND		= 2;
const int ERR_OUTPUT_NOT_OPENED	= 3;
const int ERR_INOUPUT_SAME			= 4;

const int DEF_COUNT	= 6;
const int DEF_TYPE	= 0;
const int DEF_PROC	= 1;
const int DEF_FACT	= 2;
const int DEF_PREC	= 3;
const int DEF_TRIM	= 4;
const int DEF_QUOTE	= 5;

const int COL_NUM  = 0;
const int COL_TEXT = 1;
const int COL_DATE = 2;
const int COL_TIME = 3;
const int COL_DEL  = 4;
const int PROC_FIRST = 11;
const int PROC_LAST  = 12;
const int PROC_AMEAN = 13;
const int PROC_QMEAN = 14;
const int PROC_GMEAN = 15;
const int PROC_MIN   = 16;
const int PROC_MAX   = 17;
const int PROC_SUM   = 18;
const int PROC_DIFF  = 19;
const int PROC_CLEAR = 20;

int ColType(string Str) {
	if (Str=="num")  return COL_NUM;
	if (Str=="date") return COL_DATE;
	if (Str=="time") return COL_TIME;
	if (Str=="del")  return COL_DEL;
	return COL_TEXT;
}
int ColProc(string Str) {
	if (Str=="first") return PROC_FIRST;
	if (Str=="amean") return PROC_AMEAN;
	if (Str=="qmean") return PROC_QMEAN;
	if (Str=="gmean") return PROC_GMEAN;
	if (Str=="min")   return PROC_MIN;
	if (Str=="max")   return PROC_MAX;
	if (Str=="sum")   return PROC_SUM;
	if (Str=="diff")  return PROC_DIFF;
	if (Str=="clear") return PROC_CLEAR;
	return PROC_LAST;
}

string ProcessBundle(vector<string> Col, vector<string> ColDef, string Comma, string PrevBundleValue) {
	unsigned int i;
	double res = 0;
	string sres = "";
	unsigned int Type	= ColType(ColDef[DEF_TYPE]);
	unsigned int Proc = ColProc(ColDef[DEF_PROC]);
	double Factor			= StringToDouble(ColDef[DEF_FACT]);
	int Precision			= StringToInt(ColDef[DEF_PREC]);
	bool Trim					= (ColDef[DEF_TRIM]=="yes" or ColDef[DEF_TRIM]=="1") ? true : false;
	bool Quote				= (ColDef[DEF_QUOTE]=="yes" or ColDef[DEF_QUOTE]=="1") ? true : false;

	switch(Type) {
		case COL_TEXT: {
			if (Proc==PROC_FIRST) {
				sres = Col[0];
			} else if (Proc==PROC_CLEAR) {
				sres = "";
			} else { //Proc=LAST
				sres = Col[Col.size()-1];
			}
			if (Quote) return AddQuotes(sres);
			return sres;
			break;
		}
		case COL_DATE: {
			if (Proc==PROC_FIRST) {
				sres = Col[0];
			} else if (Proc==PROC_CLEAR) {
				sres = "";
			} else { //Proc=LAST
				sres = Col[Col.size()-1];
			}
			if (Quote) return AddQuotes(sres);
			return sres;
			break;
		}
		case COL_TIME: {
			if (Proc==PROC_FIRST) {
				sres = Col[0];
			} else if (Proc==PROC_CLEAR) {
				sres = "";
			} else { //Proc=LAST
				sres = Col[Col.size()-1];
			}
			if (Quote) return AddQuotes(sres);
			return sres;
			break;
		}
		case COL_NUM: {
			if (Proc==PROC_FIRST) {
				res = StringToDouble(Col[0]);
			}
			if (Proc==PROC_LAST) {
				res = StringToDouble(Col[Col.size()-1]);
			}
			if (Proc==PROC_AMEAN or Proc==PROC_SUM) {
				for (i=0; i<Col.size(); i++) {res+=StringToDouble(Col[i]);}
				if (Proc==PROC_AMEAN) res = res / Col.size();
			}
			if (Proc==PROC_QMEAN) {
				for (i=0; i<Col.size(); i++) {res+=pow(StringToDouble(Col[i]),2);}
				res = res / Col.size();
			}
			if (Proc==PROC_GMEAN) {
				res = 1;
				for (i=0; i<Col.size(); i++) {res*=StringToDouble(Col[i]);}
				res = pow(res,1/Col.size());
			}
			if (Proc==PROC_MIN) {
				res = StringToDouble(Col[0]);
				for (i=1; i<Col.size(); i++) {
					if (StringToDouble(Col[i]) < res) res = StringToDouble(Col[i]);
				}
			}
			if (Proc==PROC_MAX) {
				res = StringToDouble(Col[0]);
				for (i=1; i<Col.size(); i++) {
					if (StringToDouble(Col[i]) > res) res = StringToDouble(Col[i]);
				}
			}
			if (Proc==PROC_DIFF) {
				res = StringToDouble(Col[Col.size()-1]) - StringToDouble(PrevBundleValue);
			}
			if (Proc==PROC_CLEAR) {
				res = 0;
			}
			sres = DoubleToString(res*Factor, Precision);
			i = sres.find(".");
			if (i!=string::npos) sres.replace(i,1,Comma);
			return sres;
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//   Hauptprogramm
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
	string config_filename		= "";
	string input_filename			= "";
	string output_filename		= "";
	string import_separator		= ";";
	string export_separator		= ";";
	string export_comma	   		= ",";
	unsigned int start_line		= 1;
	unsigned int bundle_rows	= 5;
	unsigned int bundle_cols	= 1;
	vector< vector<string> > column_def;
	vector<string> vstring;
	string line;
	unsigned int i;
	unsigned int processed_lines = 0;

	/*############################################################################
	#
	#  Konfiguration einlesen
	#
	############################################################################*/

	config_filename = ExtractFilenamePart(argv[0],true,true,false) + ".ini";
	if (argc>1) {
		if (file_exists(argv[1])) {config_filename = argv[1];}
	}
	ifstream config_file;
	config_file.open(config_filename.c_str(), ios::in);
	if (!config_file.is_open()) {
		// Programm abbrechen, weil Datei nicht gefunden
		cout << "Konfigurationsdatei '" << config_filename << "' nicht gefunden... Abgebrochen" << endl;
		return ERR_CONFIG_NOT_FOUND;
	} else {
		// Parameter auslesen
		vector<string> entries;
    while (getline(config_file, line)) {
        stringstream sline(line);
        string variable, value;
        sline>>variable>>value;
        if (variable=="import_separator")		{import_separator=value;}
        if (variable=="export_separator")		{export_separator=value;}
        if (variable=="export_comma")				{export_comma=value;}
        if (variable=="start_line")					{start_line=StringToInt(value);}
        if (variable=="row_bundle")					{bundle_rows=StringToInt(value);}
        if (variable=="input_file") {
					if (ExtractFilenamePart(value,true,false,false)=="") {value = ExtractFilenamePart(argv[0],true,false,false)+value;}
        	input_filename = value;
				}
        if (variable=="output_file") {
					if (ExtractFilenamePart(value,true,false,false)=="") {value = ExtractFilenamePart(argv[0],true,false,false)+value;}
        	output_filename = value;
				}
				if (variable=="column") {
					for (i=CountSubstr(value,","); i<DEF_COUNT; i++) {value+=",";}
					SplitString(value, ",", entries);
					column_def.push_back(entries);
				}
		}
		bundle_cols = column_def.size();
		config_file.close();
	}

	//cout << start_line << " / " << precision << " / " << import_separator << " / " << bundle_rows << " / " << bundle_cols << endl;
	//for (i=0; i<bundle_cols; i++) cout << bundle_type[i] << "-" << bundle_proc[i] << "-" << bundle_factor[i] << endl;

	/*############################################################################
	#
	#  Namen der Eingabe- und Ausgabedatei festlegen, und beide öffnen.
	#
	############################################################################*/

	cout << endl;
	cout << "Eingabedatei: " << input_filename << " -> " << ((file_exists(input_filename))?"OK":"Nicht gefunden.") << endl;
	cout << "Ausgabedatei: " << output_filename << " -> " << ((file_exists(output_filename))?"Wird überschrieben.":"OK") << endl;
	cout << endl;

	// Eingabedatei öffnen. Bei Fehler Abbruch des Programms
	ifstream input_file;
	input_file.open(input_filename.c_str(), ios::in);		// öffnet Datei im Lesemodus
	if (!input_file.is_open()) {												// Programm abbrechen, weil Datei nicht gefunden
		cout << "Eingabedatei nicht gefunden... Abgebrochen" << endl;
		return ERR_INPUT_NOT_FOUND;
	}

	// Ausgabedatei erstellen und zum Schreiben öffnen. Bei Fehler Abbruch des Programms
	ofstream output_file(output_filename.c_str(), ios::out); // öffnet Datei im Schreibmodus
	if (!output_file.is_open()) {
		cout << "Konnte Ausgabedatei nicht erstellen.... Abgebrochen" << endl;
		return ERR_OUTPUT_NOT_OPENED;
	}

	if (input_filename==output_filename) {
		cout << "Ein- und Ausgabedatei sind die selben... Abgebrochen" << endl;
		return ERR_INOUPUT_SAME;
	}

	/*############################################################################
	#
	#  Umwandlung der CSV-Datei
	#
	############################################################################*/

	// Initialisieren
	cout << "Starte Umwandlung..." << endl;
	vector<string> cols;
	vector< vector<string> > bundle;
	vector<string>* col;
	unsigned int bundle_count = 0;
	string bundle_result;
	// 2D-Vektor initialisieren
	for (i=0; i<bundle_cols; i++) {
		bundle.push_back(vector<string>());
	}

	// Header aus Eingabedate in Ausgabedatei übernehmen
	for (i=0; i<start_line-1; i++) {
		if (getline(input_file, line)) {
			output_file << line << endl;
		}
	}
	// Datenzeilen verarbeiten
	while (getline(input_file, line)) {
		bundle_count++;
		// Zeile in Vector schreiben
		SplitString(line, import_separator, cols);
		// Einträge säubern
		for (i=0; i<cols.size(); i++) {cols[i] = Trim(cols[i]);}
		// Bundle neu beginnen
		if (bundle_count==1) {
			for (i=0; i<bundle_cols; i++) {bundle[i].clear();}
		}
		// Zeile dem Bundle hinzufügen
		for (i=0; i<bundle_cols; i++) {bundle[i].push_back(cols[i]);}
		// Bundle abschließen
		if (bundle_count==bundle_rows) {
			bundle_result = "";
			for (i=0; i<bundle_cols; i++) {
				if (ColType(column_def[i][DEF_TYPE])!=COL_DEL) {
					bundle_result += ProcessBundle(bundle[i], column_def[i], export_comma, "") + export_separator;
				}
			}
			output_file << bundle_result.substr(0,bundle_result.length()-1) << endl;
			bundle_count = 0;
		}
		processed_lines++;
	}
	if (bundle_count!=0) {
		bundle_result = "";
		for (i=0; i<bundle_cols; i++) {
			if (ColType(column_def[i][DEF_TYPE])!=COL_DEL) {
				bundle_result += ProcessBundle(bundle[i], column_def[i], export_comma, "") + export_separator;
			}
		}
		output_file << bundle_result.substr(0,bundle_result.length()-1) << endl;
	}

	/*############################################################################
	#
	#  Beenden des Programm
	#
	############################################################################*/

	input_file.close();
	output_file.close();
	cout  << processed_lines << " verarbeitete Zeilen." << endl;
	cout << "Umwandlung abgeschlossen." << endl << endl;
	return 0;
}
