
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include <chrono>
#include <array>


using namespace std;

//classes
class Robot{
    int posX, posY, fitness, power, pickedGene, maxGenes = 16;
    //[0] = n, [1] = s, [2] = e, [3] = w
    //0 = nothing, 1 = wall, 9 = battery
    int sensor[4];
    int genes[16][5];
    


    public:
    int RandomGene(){
        

        switch(rand() % 3){
            case 0:
            return 0;
            break;
            case 1:
            return 1;
            break;
            case 2:
            return 9;
            break;
        }
        return 0;
    }
    void RestartBot(){
        posX = rand() % 12;
        posY = rand() % 12;
        power = 5;
        fitness = 0;

    }
    int RandomDirection(){
        return rand() % 4;
    }
    void InitGenes(){
        for(int x=0; x<maxGenes; x++){
            for(int y=0; y<4; y++){
                genes[x][y] = RandomGene();
            }
            genes[x][4] = RandomDirection();
        }
    }
    int GetGenes(int geneNum, int gene){
        return genes[geneNum][gene];
    }
    void CheckFitness(){
        if(fitness > 290 || fitness < 0){
            fitness = 0;
        }
    }
    void SetGenes(int geneNum, int gene, int code){
        genes[geneNum][gene] = code;
    }
    Robot(){
        
        posX = rand() % 12;
        posY = rand() % 12;
        power = 5;
    }
    void Move(){
        if(genes[pickedGene][4] == 0 && sensor[0] != 1){
            posX-=1;
            
        }else if(genes[pickedGene][4] == 1 && sensor[1] != 1){
            posX+=1;
            
        }else if(genes[pickedGene][4] == 2 && sensor[2] != 1){
            posY+=1;
            
        }else if(genes[pickedGene][4] == 3 && sensor[3] != 1){
            posY-=1;
            
        }
        fitness++;
        power--;

    }
    
    void CheckForBattery(int board[12][12]){
        if(board[posX][posY] == 9){
            power+=5;
            board[posX][posY] = 0;
        }
    }
    int GetFitness(){
        return fitness;
    }
    int GetPower(){
        return power;
    }

    void CompareGeneAndSensor(){

        for(int x=0; x<maxGenes; x++){
            pickedGene = 0;
            if(genes[x][0] == sensor[0] && genes[x][1] == sensor[1] && genes[x][2] == sensor[2] && genes[x][3] == sensor[3]){
                pickedGene = x;
                break;
            }else{
                pickedGene = 15;
            }
        }
    }
    void UpdateSensor(int board[12][12]){
        //check for walls
        if(posX == 0){
            sensor[0] = 1;
        }
        if(posX == 11){
            sensor[1] = 1;
        }
        if(posY == 11){
            sensor[2] = 1;
        }
        if(posY == 0){
            sensor[3] = 1;
        }

        //check for batteries
        if(sensor[0] != 1){
            if(board[posX-1][posY] == 9){
                sensor[0] = 9;
            }else{
                sensor[0] = 0;
            }
        }
        if(sensor[1] != 1){
            if(board[posX+1][posY] == 9){
                sensor[1] = 9;
            }else{
                sensor[1] = 0;
            }
        }
        if(sensor[2] != 1){
            if(board[posX][posY+1] == 9){
                sensor[2] = 9;
            }else{
                sensor[2] = 0;
            }
        }
        if(sensor[3] != 1){
            if(board[posX][posY-1] == 9){
                sensor[3] = 9;
            }else{
                sensor[3] = 0;
            }
        }

    }

};
class Map{
    

};

//function protos
void MakeBoard();
void SimLoop();
void ReproduceBots(Robot bots[200]);
void Sort(Robot bots[200]);
void Swap(Robot *x, Robot *y);
void DisplayStats(Robot bots[200], int gen, double averages[100]);
void MakeGraph(double averages[100]);

int main(){

    srand(time(NULL));

    SimLoop();
    

    return 0;
}

//fucntion defs
void MakeBoard(int board[12][12], int maxBattery){
    int numOfBatteries = 0;
    for(int x=0; x<12; x++){
        for(int y=0; y<12; y++){
            board[x][y] = 0;
        }
    }
    while(numOfBatteries <= maxBattery){
        int randNum1 = rand() % 12;
        int randNum2 = rand() % 12;
        if(board[randNum1][randNum2] != 9){
            board[randNum1][randNum2] = 9;
            numOfBatteries++;
        }
    }
}
void SimLoop(){
    Robot bots[200];
    double averages[100];
    for(int x=0; x<196; x++){
        bots[x].InitGenes();
    }

    for (int x=0; x<=100; x++){
        for(int y=0; y<200; y++){
            int board[12][12];
            int maxBattery = 58;
            MakeBoard(board, maxBattery);
            while(bots[y].GetPower() > 0){
                this_thread::sleep_for(chrono::nanoseconds(3));
                this_thread::sleep_until(chrono::system_clock::now() + chrono::nanoseconds(10));
                bots[y].CheckForBattery(board);
                bots[y].CheckFitness();
                bots[y].UpdateSensor(board);
                bots[y].CompareGeneAndSensor();
                bots[y].Move();
            }
        }
        DisplayStats(bots, x, averages);
        Sort(bots);
        for(int i=0; i<99; i++){
            bots[i].RestartBot();
        }
        ReproduceBots(bots);

    }
    MakeGraph(averages);
}
void ReproduceBots(Robot bots[200]){
    Sort(bots);
    int mutationPerc = 5;
    int newGen = 100;
    for(int x=199; x>100; x = x-2){
        for(int geneNum = 0; geneNum < 8; geneNum++){
            for(int gene = 0; gene < 5; gene++){
                if(rand() % 100+1 <= mutationPerc){
                    bots[newGen].SetGenes(geneNum, gene, bots[x].RandomGene());
                }else{
                    bots[newGen].SetGenes(geneNum, gene, bots[x].GetGenes(geneNum,gene));
                }
                
            }
        }
        for(int geneNum = 8; geneNum < 16; geneNum++){
            for(int gene = 0; gene < 5; gene++){
                if(rand() % 100+1 <= mutationPerc){
                    bots[newGen+1].SetGenes(geneNum, gene, bots[x].RandomGene());
                }else{
                    bots[newGen+1].SetGenes(geneNum, gene, bots[x].GetGenes(geneNum,gene));
                }
                
            }
        }
        for(int geneNum = 0; geneNum < 8; geneNum++){
            for(int gene = 0; gene < 5; gene++){
                if(rand() % 100+1 <= mutationPerc){
                    bots[newGen+1].SetGenes(geneNum, gene, bots[x+1].RandomGene());
                }else{
                    bots[newGen+1].SetGenes(geneNum, gene, bots[x+1].GetGenes(geneNum,gene));
                }
                
            }
        }
        for(int geneNum = 8; geneNum < 16; geneNum++){
            for(int gene = 0; gene < 5; gene++){
                if(rand() % 100+1 <= mutationPerc){
                    bots[newGen].SetGenes(geneNum, gene, bots[x+1].RandomGene());
                }else{
                    bots[newGen].SetGenes(geneNum, gene, bots[x+1].GetGenes(geneNum,gene));
                }
                
            }
        }
        newGen--;
    }
}
void Sort(Robot bots[200]){
    for(int x=0; x<200-1; x++){
        for(int j=0; j<200-x-1; j++){
            if(bots[j].GetFitness() > bots[j+1].GetFitness()){
                Swap(&bots[j], &bots[j+1]);
            }
        }
    }
}
void Swap(Robot *x, Robot *y){
    Robot temp = *x;
    *x = *y;
    *y = temp;
}
void DisplayStats(Robot bots[200], int gen, double averages[100]){
    double average = 0, temp = 0, it = 0;
    for(int x=0; x<200; x++){
        temp = temp + bots[x].GetFitness();
        it++;
    }
    average = temp / 200;
    averages[gen] = average * 2;
    cout << "the average fitness score for Gen " << gen << " was " << average << endl;
}
void MakeGraph(double averages[100]){
    cout << "averages graph" << endl;
    for(int x = 0; x<100; x++){
        for(int y=0; y<averages[x]; y++){
            cout << " ";
        }
        cout << "*" << endl;
    }
}
