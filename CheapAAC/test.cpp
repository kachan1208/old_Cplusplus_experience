#include <map>
#include <iostream>

struct Atom
{
    int    start;
    int    len;
    char*  data;
};

typedef std::map<int, Atom> AtomMap;
AtomMap mAtomMap;

int main()
{
//-------1--------- 
    Atom atom1; 
    Atom atom2;
    atom1.start = 1;
    atom2.start = 2;
    atom1.len = 10;
    atom2.len = 20;
    atom1.data = (char*)"atom1";
    atom2.data = (char*)"atom2";
    mAtomMap[1] = atom1;
    mAtomMap[2] = atom2;
    std::cout << "\n-----standart-----\natom1\tstart: "<<atom1.start<<"\tlen: "<<atom1.len<<"\tdata: "<<atom1.data;
    std::cout << "\natom2\tstart: "<<atom2.start<<"\tlen: "<<atom2.len<<"\tdata: "<<atom2.data;
    std::cout << "\nmAtomMap atom1\tstart: "<<mAtomMap[1].start<<"\tlen: "<<mAtomMap[1].len<<"\tdata: "<<mAtomMap[1].data;
    std::cout << "\nmAtomMap atom2\tstart: "<<mAtomMap[2].start<<"\tlen: "<<mAtomMap[2].len<<"\tdata: "<<mAtomMap[2].data;
    Atom atom = mAtomMap[1];
    atom.len = -1;
    std::cout << "\n\n-------Atom atom = mAtomMap[1]; atom.len = -1;--------\n\natom1\tstart: "<<atom1.start<<"\tlen: "<<atom1.len<<"\tdata: "<<atom1.data;
    std::cout << "\natom\tstart: "<<atom.start<<"\tlen: "<<atom.len<<"\tdata: "<<atom.data;    
    std::cout << "\nmAtomMap atom1\tstart: "<<mAtomMap[1].start<<"\tlen: "<<mAtomMap[1].len<<"\tdata: "<<mAtomMap[1].data;
    mAtomMap[2].len = -2;
    std::cout << "\n\n---------mAtomMap[2].len = -2------\n\nmAtomMap atom2\tstart: "<<mAtomMap[2].start<<"\tlen: "<<mAtomMap[2].len<<"\tdata: "<<mAtomMap[2].data;
    std::cout << "\natom2\tstart: "<<atom2.start<<"\tlen: "<<atom2.len<<"\tdata: "<<atom2.data;
    Atom& atom3 = mAtomMap[1]; atom3.len = -1; 
    std::cout << "\n\n---------Atom& atom3 = mAtomMap[1]; atom3.len = -1;-------\n\natom1\tstart: "<<atom3.start<<"\tlen: "<<atom3.len<<"\tdata: "<<atom3.data;
    std::cout << "\nmAtomMap atom1\tstart: "<<mAtomMap[1].start<<"\tlen: "<<mAtomMap[1].len<<"\tdata: "<<mAtomMap[1].data;


}
