/* This code is a modified version of Boyar's original implementation of the SLP heuristic*/
#include <math.h>
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

using namespace std;

const int MaxBaseSize=1000;
const bool PRINTROWS=true;

int NumInputs;
int DepthLimit; //The depth bound of the output circuit
int NumMatrices;
int MaxDist; // The number of 1s in the row of the matrix with the maximum number of 1s
int St; //sequence number of the matrices
long long int DepthNewBase; //depth of a new base which is used for calculating the new distances
int NumTargets;
int ProgramSize;
long long int Target[MaxBaseSize];
int Dist[MaxBaseSize]; //distance from current base to Target[i]
int NDist[MaxBaseSize]; //what Dist would be if NewBase is added
long long int Base[MaxBaseSize];
int BaseSize;
int TargetsFound;
char Result[MaxBaseSize][50];
int  Res;
char *flag;
int Depth[MaxBaseSize];
int MaxDepth;

void InitBase();
void ReadTargetMatrix();
bool is_target(long long int x);
bool is_base(long long int x);
int NewDistance(int u); //calculate the distance from the base to Target[u]
int TotalDistance(); //returns the sum of distances to targets
bool reachablE(long long int T, int K, int S, long long int L);
bool EasyMove(); //if any two bases add up to a target, pick them
void PickNewBaseElement();
void binprint(long long int x); //output the last NumInputs bits of x

void PrintExpression(int No);
void PrintBase(void);
void PrintResult();
int Max(int a, int b);

ifstream TheMatrix;

int
main(int argc, char *argv[])
{
  int Threshold;
  int sum;
  int order[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  int order1_2[16] = {0, 1, 6, 7, 2, 3, 4, 5, 8, 9, 14, 15, 10, 11, 12, 13};
  int order1_3[16] = {1, 0, 5, 4, 6, 7, 2, 3, 9, 8, 13, 12, 14, 15, 10, 11};
  int order2_2[16] = {1, 0, 6, 7, 3, 2, 4, 5, 9, 8, 14, 15, 11, 10, 12, 13};
  int order2_3[16] = {1, 0, 4, 5, 7, 6, 2, 3, 9, 8, 12, 13, 15, 14, 10, 11};
  int U[16], B[16], i;

  clock_t t1=clock();
  TheMatrix.open("test.txt");

  TheMatrix >> NumMatrices;
  TheMatrix >> Threshold;
  TheMatrix >> DepthLimit;
  for (i = 0; i < NumMatrices; i++)
  {
    ReadTargetMatrix();

    if (MaxDist + 1 > pow(2, DepthLimit)) continue;
    InitBase();
    ProgramSize = 0;
    int counter = 0;
    St = i+1;

    while (TargetsFound < NumTargets)
    {
	  counter++;
	  if(ProgramSize > Threshold)
	  {
		break;
	  }
	  if (!EasyMove()) PickNewBaseElement();
    }

    if(ProgramSize <= Threshold)
    	PrintResult();
    else
    	printf("Out of threshold\n\n");

  }

  clock_t t2=clock();
  cout << (t2-t1)/(double)CLOCKS_PER_SEC;
}

void InitBase()
{
	TargetsFound = 0;
	Res = 0;
	Base[0] = 1;
	Depth[0] = 0;
	MaxDepth = 0;
	for (int i = 1; i < NumInputs; i++)
	{
		Base[i] = 2*Base[i-1];
		Depth[i] = 0;
	}
	BaseSize = NumInputs; //initial base is just the xi's, depths are 0
	for (int i = 0; i < NumTargets; i++)
	    if (Dist[i] == 0)
		{
			TargetsFound++;
			//print the expression of the output and input
			for(int j = 0; j < NumInputs; ++j)
				if(Base[j] == Target[i])
				{
					sprintf(Result[Res], "y%d = x%d  *  (0)\n", i, j);
					++Res;
					break;
				}
		}
}

int TotalDistance() //returns the sum of distances to targets
{
  int D = 0;
  int t;
  for (int i = 0; i < NumTargets; i++)
  {
    t = NewDistance(i);
    NDist[i] = t;
    D = D + t;
  }
  return D;
}

long long int NewBase; //global variable containing a candidate new base

bool EasyMove()
{
  int t;
  bool foundone = false;

  //see if anything in the distance vector is 1
  for(int i = 0; i < NumTargets; i++)
    if (Dist[i] == 1)
    {
      foundone = true;
      t = i;
      break;
    }
  if (!foundone) return false;
  //update Dist array
  NewBase = Target[t];
  Base[BaseSize] = NewBase;
  BaseSize++;
  DepthNewBase = pow(2, DepthLimit);
  for(int i = 0; i < BaseSize; ++i)
		for(int j = i + 1; j < BaseSize; ++j)
			if(((Base[i] ^ Base[j]) == Base[BaseSize - 1]) && (DepthNewBase > pow(2, Max(i,j)+1)))
			{
				DepthNewBase = pow(2, Max(i,j)+1); //find the lowest depth of output signal i
		    }


  for (int u = 0; u < NumTargets; u++) Dist[u] = NewDistance(u);
  ProgramSize++;
  //print the expression
  PrintExpression(t);
  TargetsFound++;
  return true;
} //EasyMove()

void PrintResult()
{
	std::fstream f("result.depthlimit.txt", std::fstream::out | std::fstream::app);
	f << St << endl << endl;
	f << ProgramSize << endl << endl;
	for(int i = 0; i < Res; ++i)
		f << Result[i] ;
	f << "Depth is " << MaxDepth << endl << endl;
}

int Max(int a, int b)
{
	if(Depth[a] > Depth[b])
		return Depth[a];
	else
		return Depth[b];
}

/* print the expression*/
void PrintExpression(int No)
{
	int i, j;

	for(i = 0; i < BaseSize; ++i)
		for(j = i + 1; j < BaseSize; ++j)
			if(((Base[i] ^ Base[j]) == Base[BaseSize-1]) && (pow(2, Max(i,j)+1) == DepthNewBase))
			{
				Depth[BaseSize-1] = Max(i,j)+1;
				if(Depth[BaseSize-1] > MaxDepth)
					MaxDepth = Depth[BaseSize-1];
				flag = Result[Res];
				flag += sprintf(flag, "t%d = ", ProgramSize);
				if(i < NumInputs)
					flag += sprintf(flag, "x%d + ", i);
				else
					flag += sprintf(flag, "t%d + ", i - NumInputs + 1);
				if(j < NumInputs)
					flag += sprintf(flag, "x%d *  y%d  (%d)\n", j, No, Depth[BaseSize-1]);
				else
					flag += sprintf(flag, "t%d *  y%d  (%d)\n", j - NumInputs + 1, No, Depth[BaseSize-1]);
				++Res;
				return;
			}
}

/* PickNewBaseElement is only called when there are no 1's in Dist[]*/
void PickNewBaseElement()
{
	int MinDistance;
	long long int TheBest;
	int ThisDist;
	int ThisNorm, OldNorm;
	int besti,bestj, d;
	bool easytarget;
	int BestDist[MaxBaseSize];

	MinDistance = BaseSize*NumTargets; //i.e. something big
	OldNorm = 0; //i.e. something small
	//try all pairs of bases
	for (int i = 0; i < BaseSize - 1; i++)
	{
		if (Depth[i] + 1 >= DepthLimit) continue;
		for (int j = i+1; j < BaseSize; j++)
		{
			if (Depth[j] + 1 >= DepthLimit) continue;
			NewBase = Base[i] ^ Base[j];

			if (NewBase == 0) { cout << "a base is 0, should't happen " << endl; exit(0); }
			//if NewBase is not new continue
			if (is_base(NewBase)) continue;
			//if NewBase is target then choose it
			easytarget = false;
			if (is_target(NewBase))
			{
				cout << "shouldn't find an easy target here " << endl;
				exit(0);
				easytarget = true;
				besti = i;
				bestj = j;
				TheBest = NewBase;
				break;
			}
			DepthNewBase = pow(2, Max(j,i)+1);
			ThisDist = TotalDistance(); //this also calculates NDist[]
			if (ThisDist <= MinDistance)
			{
				//calculate Norm
				ThisNorm = 0;
				for (int k = 0; k < NumTargets; k++)
				{
					d = NDist[k];
					ThisNorm = ThisNorm + d*d;
				}
				//resolve tie in favor of largest norm
				if ((ThisDist < MinDistance) || (ThisNorm > OldNorm) )
				{
					besti = i;
					bestj = j;
					TheBest = NewBase;
					for (int uu = 0; uu < NumTargets; uu++) BestDist[uu] = NDist[uu];
					MinDistance = ThisDist;
					OldNorm = ThisNorm;
				}
			}
		}
	if (easytarget) break;
	}
	//update Dist array
	NewBase = TheBest;
	for (int i = 0; i < NumTargets; i++) Dist[i] = BestDist[i];
	//for (int i = 0; i < NumTargets; i++)
	//printf ("%d %d %d \n", besti, bestj, MinDistance);
	//std::cout<<"Press [ENTER] to continue\n";
    //std::cin.get();
	Base[BaseSize] = TheBest;
	Depth[BaseSize] = Max(besti, bestj) + 1;
	if(Depth[BaseSize] > MaxDepth)
		MaxDepth = Depth[BaseSize];
	BaseSize++;
	//output linear program
	ProgramSize++;
	//print the expression
	flag = Result[Res];
	flag += sprintf(flag, "t%d = ", ProgramSize);
	if(besti < NumInputs)
		flag += sprintf(flag, "x%d + ", besti);
	else
		flag += sprintf(flag, "t%d + ", besti - NumInputs + 1);
	if(bestj < NumInputs)
		flag += sprintf(flag, "x%d  (%d)\n", bestj, Depth[BaseSize-1]);
	else
		flag += sprintf(flag, "t%d  (%d)\n", bestj - NumInputs + 1, Depth[BaseSize-1]);
	++Res;
	if (is_target(TheBest)) TargetsFound++; //this shouldn't happen
} //PickNewBaseElement()

void binprint(long long int x) //outputs last NumInputs bits of x
{
  long long int t = x;
  for (int i = 0; i < NumInputs; i++)
  {
    if (t%2) cout << "1 "; else cout << "0 ";
    t = t/2;
  }
} //binprint

void PrintBase()
{
	int i;

	for(i = 0; i < BaseSize; ++i)
	{
		binprint(Base[i]);
		printf("\n");
	}
}

void ReadTargetMatrix()
{
  TheMatrix >> NumTargets;
  TheMatrix >> NumInputs;
  MaxDist = 0;
  //check that NumInputs is < wordsize
  if (NumInputs >= 8*sizeof(long long int))
  {
    cout << "too many inputs" << endl;
    exit(0);
  }

  int bit;
  for (int i = 0; i < NumTargets; i++)
  //read row i
  {
    long long int PowerOfTwo  = 1;
    Target[i] = 0;
    Dist[i] = -1; //initial distance from Target[i] is Hamming weight - 1
    TheMatrix.ignore(1, '[');
    for (int j = 0; j < NumInputs; j++)
    {
      TheMatrix >> bit;
      if (bit)
      {
        Dist[i]++;
		Target[i] = Target[i] + PowerOfTwo;
      }
      PowerOfTwo = PowerOfTwo * 2;
    }
    if (Dist[i] > MaxDist) MaxDist = Dist[i];
	TheMatrix.get();
  }
}


bool is_target(long long int x)
{
  for (int i = 0; i < NumTargets; i++)
    if (x == Target[i]) return true;
  return false;
}

bool is_base(long long int x)
{
  //sanity check, shouldn't ask if 0 is base
  if (x==0) { cout << "asking if 0 is in Base " <<endl ; exit(0); }

  for (int i = 0; i < BaseSize; i++) if (x == Base[i]) return true;
  return false;
}



int NewDistance(int u)
{

  if (Target[u] == 0) return 0;
  else
  {
    if (is_base(Target[u]) || (NewBase == Target[u])) return 0;



    if (reachablE(Target[u] ^ NewBase, Dist[u]-1, 0, pow(2, DepthLimit) - DepthNewBase)) return (Dist[u]-1);
    else return Dist[u];
  }
}


//return true if T is the sum of K elements among Base[S..BaseSize-1]
bool reachablE(long long int T, int K, int S, long long int L)
{
	if ((BaseSize-S) < K) return false; //not enough base elements
    if (L < 1) return false;
    if (K==0) return false; //this is probably not reached
    if (K==1)
    {
      for (int i=S; i < BaseSize; i++) if ((T == Base[i]) && (pow(2, Depth[i]) <= L)) return true;
      return false;
    }

    //consider those sums containing Base[S]
    if (reachablE(T^Base[S], K-1, S+1, L - pow(2, Depth[S]))) return true;
    //consider those sums not containing Base[S]
    if (reachablE(T, K, S+1, L)) return true;
    //not found
    return false;
}



