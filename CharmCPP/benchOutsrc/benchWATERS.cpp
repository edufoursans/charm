#include "TestWATERS.h"

string createString(int i)
{
	stringstream ss;
	ss << "ATTR" << i;
	return ss.str();
}

void getAttributes(CharmListStr & attrList, int max)
{
	for(int i = 1; i <= max; i++) {
		attrList.append(createString(i));
	}
}

string getPolicy(int max)
{
	string policystr;
	if(max >= 2) {
		policystr = "(" + createString(1) + " and " + createString(2) + ")";
	}
	else if(max == 1) {
		policystr = createString(1);
	}

	for(int i = 3; i <= max; i++)
	{
		policystr = "(" + policystr + " and " + createString(i) + ")";
	}

	return policystr;
}

void benchmarkWATERS(Waters09 & waters, ofstream & outfile1, ofstream & outfile2, int attributeCount, int iterationCount, CharmListStr & transformResults, CharmListStr & decoutResults)
{
	Benchmark benchT, benchD;
    CharmList msk, pk, skBlinded, ct, transformOutputList;
    CharmListStr S;
    GT M, newM;
    ZR bf0;
    double tf_in_ms, de_in_ms;

    waters.setup(msk, pk);
    getAttributes(S, attributeCount);
    waters.keygen(pk, msk, S, bf0, skBlinded);

    M = waters.group.random(GT_t);
    string policy_str =  getPolicy(attributeCount); // get a policy string
    waters.encrypt(pk, M, policy_str, ct);

    stringstream s1, s2;

    //cout << "ct =\n" << ct << endl;
	for(int i = 0; i < iterationCount; i++) {
		benchT.start();
		waters.transform(pk, skBlinded, S, ct, transformOutputList);
		benchT.stop();
		tf_in_ms = benchT.computeTimeInMilliseconds();
		//cout << "transformCT =\n" << transformOutputList << endl;

		benchD.start();
		waters.decout(pk, S, transformOutputList, bf0, newM);
		benchD.stop();
		de_in_ms = benchD.computeTimeInMilliseconds();
	}

	cout << "Transform avg: " << benchT.getAverage() << endl;
	s1 << attributeCount << " " << benchT.getAverage() << endl;
	outfile1 << s1.str();
    transformResults[attributeCount] = benchT.getRawResultString();

	cout << "Decout avg: " << benchD.getAverage() << endl;
	s2 << attributeCount << " " << benchD.getAverage() << endl;
	outfile2 << s2.str();
	decoutResults[attributeCount] = benchD.getRawResultString();
//    cout << convert_str(M) << endl;
//    cout << convert_str(newM) << endl;
    if(M == newM) {
      cout << "Successful Decryption!" << endl;
    }
    else {
      cout << "FAILED Decryption." << endl;
    }
    return;
}

int main(int argc, const char *argv[])
{
	string FIXED = "fixed", RANGE = "range";
	if(argc != 4) { cout << "Usage " << argv[0] << ": [ iterationCount => 10 ] [ attributeCount => 100 ] [ 'fixed' or 'range' ]" << endl; return -1; }

	int iterationCount = atoi( argv[1] );
	int attributeCount = atoi( argv[2] );
	string fixOrRange = string(argv[3]);
	cout << "iterationCount: " << iterationCount << endl;
	cout << "attributeCount: " << attributeCount << endl;
	cout << "measurement: " << fixOrRange << endl;

	Waters09 waters;
	string filename = string(argv[0]);
	stringstream s3, s4;
	ofstream outfile1, outfile2, outfile3, outfile4;
	string f1 = filename + "_transform.dat";
	string f2 = filename + "_decout.dat";
	string f3 = filename + "_transform_raw.txt";
	string f4 = filename + "_decout_raw.txt";
	outfile1.open(f1.c_str());
	outfile2.open(f2.c_str());
	outfile3.open(f3.c_str());
	outfile4.open(f4.c_str());

	CharmListStr transformResults, decoutResults;
	if(isEqual(fixOrRange, RANGE)) {
		for(int i = 2; i <= attributeCount; i++) {
			cout << "Benchmark with " << i << " attributes." << endl;
			benchmarkWATERS(waters, outfile1, outfile2, i, iterationCount, transformResults, decoutResults);
		}
		s3 << transformResults << endl;
		s4 << decoutResults << endl;
	}
	else if(isEqual(fixOrRange, FIXED)) {
		cout << "Benchmark with " << attributeCount << " attributes." << endl;
		benchmarkWATERS(waters, outfile1, outfile2, attributeCount, iterationCount, transformResults, decoutResults);
		s3 << attributeCount << " " << transformResults[attributeCount] << endl;
		s4 << attributeCount << " " << decoutResults[attributeCount] << endl;
	}
	else {
		cout << "invalid option." << endl;
		return -1;
	}

	outfile3 << s3.str();
	outfile4 << s4.str();
	outfile1.close();
	outfile2.close();
	outfile3.close();
	outfile4.close();
//	cout << "<=== Transform benchmarkWATERS breakdown ===>" << endl;
//	cout << transformResults << endl;
//	cout << "<=== Transform benchmarkWATERS breakdown ===>" << endl;
//	cout << "<=== Decout benchmarkWATERS breakdown ===>" << endl;
//	cout << decoutResults << endl;
//	cout << "<=== Decout benchmarkWATERS breakdown ===>" << endl;

	return 0;
}

