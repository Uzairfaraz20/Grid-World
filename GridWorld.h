
#include <stdio.h>
#include <stdlib.h>
#include <cstddef>
#include "GWInterface.h"
#include <vector>
#include <iostream>

using std::vector;


class GridWorld : public GWInterface {

  public:

    GridWorld(unsigned nrows, unsigned ncols)   {
	
		totalPopulation = 0;
		idCounter = -1;
		gridRows = nrows;
		gridCols = ncols;

		
		World = new District**[nrows];
		for (int i = 0; i < nrows; i++) {
			World[i] = new District*[ncols];
		}


		for (int x = 0; x < gridRows; x++) {
			for (int y = 0; y < gridCols; y++) {
				District* newDistrict = new District();
				newDistrict->row = x;
				newDistrict->col = y;
				World[x][y] = newDistrict;
			}
		}

		People = new std::vector<Person*>;
		GraveYard = new std::vector<Person*>;

	

    }
    ~GridWorld(){
		delete(World);
		delete(People);
		delete(GraveYard);
    }

    
	
	bool birth(int row, int col, int &id){
		
		if (row < 0 || row >= gridRows) {
			return false;
		}

		if (col < 0 || col >= gridCols) {
			return false;
		}
		
		Person* lastPerson;
		
		if (GraveYard->size() > 0) {
			lastPerson = GraveYard->front();
			GraveYard->erase(GraveYard->begin());
			id = lastPerson->id;
		}
		else {
			lastPerson = new Person();
			id = ++idCounter;
			lastPerson->id = id;
		}
		
		lastPerson->row = row;
		lastPerson->col = col;
		
		District* district = World[row][col];
			
		
			if (district->front == nullptr) {
			district->people = lastPerson;
			district->front = lastPerson;
			district->back = lastPerson;
		}
		else
		{
			district->back->next = lastPerson;
			lastPerson->prev = district->back;
			district->back = lastPerson;
		}

		district->districtPopulation++;
		People->push_back(lastPerson);
		totalPopulation++;

		return true;
    }

    bool death(int personID){

		if (personID > People->size()) {
			return false;
		}

		Person* personToDie = People->at(personID);
		if (personToDie->row == -1 && personToDie->col == -1) {
			return false;
		}

		Person* prevPerson = personToDie->prev;
		Person* nextPerson = personToDie->next;
		District* myDistrict = World[personToDie->row][personToDie->col];

		if (prevPerson == nullptr) {
			if (nextPerson != nullptr) {
				nextPerson->prev = nullptr;
			}
			
			
			myDistrict->front = nextPerson;
			myDistrict->people = nextPerson;
			personToDie->next = nullptr;
			
		}
		else if(nextPerson == nullptr) {
			personToDie->prev = nullptr;
			myDistrict->back = prevPerson;
			prevPerson->next = nullptr;
		}
		else {
			prevPerson->next = nextPerson;
			nextPerson->prev = prevPerson;
		}

		personToDie->row = -1;
		personToDie->col = -1;
		personToDie->next = nullptr;
		personToDie->prev = nullptr;
	
		GraveYard->push_back(personToDie);
		totalPopulation--;
		myDistrict->districtPopulation--;
		

		
		
		return true;
    }

    bool whereis(int id, int &row, int &col)const{
		if (People->size() <= id) {
			return false;
		}
		Person* person = People->at(id);
		if (person->row == -1 && person->col == -1) {
			return false;
		}
		row = person->row;
		col = person->col;

		return true;
    }

    bool move(int id, int targetRow, int targetCol){
      
		if (targetRow < 0 || targetRow >= gridRows) {
			return false;
		}
		if (targetCol < 0 || targetCol >= gridCols) {
			return false;
		}

		if (People->size() < id) {
			return false;
		}

		Person* personToMove = People->at(id);

		District* fromDistrict = World[personToMove->row][personToMove->col];
		District* toDistrict = World[targetRow][targetCol];
		
		//Move out person from district

		Person* prevPerson = personToMove->prev;
		Person* nextPerson = personToMove->next;

		if (prevPerson == nullptr) {
			if (nextPerson!=nullptr) {
				nextPerson->prev = nullptr;
			}
			
			District* myDistrict = World[personToMove->row][personToMove->col];
			myDistrict->front = nextPerson;
			myDistrict->people = nextPerson;
			personToMove->next = nullptr;
			
		}
		else if(nextPerson == nullptr) {
			personToMove->prev = nullptr;
			District* myDistrict = World[personToMove->row][personToMove->col];
			myDistrict->back = prevPerson;
			prevPerson->next = nullptr;
		}
		else {
			prevPerson->next = nextPerson;
			nextPerson->prev = prevPerson;
		}

		fromDistrict->districtPopulation--;

		//move a person to new district

		if (toDistrict->front == nullptr) {
			toDistrict->people = personToMove;
			toDistrict->front = personToMove;
			toDistrict->back = personToMove;

			personToMove->next = nullptr;
			personToMove->prev = nullptr;
		}
		else
		{
			toDistrict->back->next = personToMove;
			personToMove->prev = toDistrict->back;
			toDistrict->back = personToMove;
			personToMove->next = nullptr;
		}
		

		toDistrict->districtPopulation++;
		personToMove->row = targetRow;
		personToMove->col = targetCol;

		
		return true;
    }

    std::vector<int> * members(int row, int col)const{

		std::vector<int> * memberIds = new std::vector<int>();
		
		if (row < 0 || row >= gridRows) {
			return memberIds;
		}

		if (col < 0 || col >= gridCols) {
			return memberIds;
		}


		District* myDistrict = World[row][col];
		Person* curr = myDistrict->people;

		while (curr != nullptr) {
			memberIds->push_back(curr->id);
			curr = curr->next;
		}
      
		
		return memberIds;
    }

    int population()const{
      return totalPopulation;
    }
    
    int population(int row, int col)const{
		
		if (row < 0 || row >= gridRows) {
			return 0;
		}

		if (col < 0 || col >= gridCols) {
			return 0;
		}
		District* district = World[row][col];
		return district->districtPopulation;
    }

    int num_rows()const {
      return gridRows;
    }

    int num_cols()const {
      return gridCols;
    }


  private:

	  typedef struct LinkedList
	  {
		 int id;
		 int row;
		 int col;
		 LinkedList *next;
		 LinkedList *prev;
	  
	  } Person;

	  typedef struct district
	  {
		  Person* people;
		  int row;
		  int col;
		  int districtPopulation;
		  Person* front;
		  Person* back;
	  } District;

	  
	  //Main GridWorld
	  District*** World;

	  //People List
	  std::vector<Person*> * People;

	  //Graveyard
	  std::vector<Person*> * GraveYard;

	  //Population and ID counters
	  int totalPopulation;
	  int idCounter;

	  int gridRows;
	  int gridCols;


};
