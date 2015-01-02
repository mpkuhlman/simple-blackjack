#include "deck.h"

const int NUM_SPOTS_IN_SUIT = 13;

Deck::Deck()
{
  reset();
}

void Deck::reset()
{
  Spot sp;
  Suit st = SPADES;

  for(int i = 0; i <= DIAMONDS; i++) { 
    sp = TWO; 
    for(int j = 0; j <= ACE; j++) {
      deck[(NUM_SPOTS_IN_SUIT * i) + j].spot = sp;
      deck[(NUM_SPOTS_IN_SUIT * i) + j].suit = st;
      sp = static_cast<Spot>(sp + 1);
    }
    st = static_cast<Suit>(st + 1);
  }
  next = 0;  // Two of Spades first card to deal
}


void Deck::shuffle(int n)
{
  int left = 0;
  int right = 0;
  int count = 0;
  bool useRightSide = true;
    
  if(n > 0 && n < DeckSize) {  // No changes to deck if n = 0 or n = 52
    Card leftSide[DeckSize];

    for(int x = 0; x < n; x++) {
      leftSide[x] = deck[x];
    }
    
    while(count < DeckSize) { 
      if(useRightSide && ( (n + right) < DeckSize) ) {
	deck[count] = deck[n + right];
	right++;
	count++;
      }

      else if(!useRightSide && (left < n)) {
	deck[count] = leftSide[left];
	left++;
	count++;
      }
      useRightSide = !useRightSide;
    }
  }
  next = 0;
}

Card Deck::deal()
{
  DeckEmpty d;
  if(cardsLeft() == 0)
    throw(d);
  else {
    next++;
    return deck[next - 1];
  }
}


int Deck::cardsLeft()
{
  return DeckSize - next;  
}
