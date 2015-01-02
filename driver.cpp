#include <iostream>
#include <cstdlib>
#include <string>
#include "hand.h"
#include "player.h"
#include "deck.h"
#include "rand.h"

using namespace std;

const int MINBET = 5;
const int CUTS_PER_SHUFFLE = 7;

//MODIFIES: The deck
//EFFECTS: Shuffles the deck at a random cut seven times, and prints where it was cut to the screen
void shuffleDeck(Deck &de)
{
  int cut;
  cout << "Shuffling the deck" << endl;
  
  for(int i = 0; i < CUTS_PER_SHUFFLE; i++) {
    cut = get_cut();
    de.shuffle(cut);
    cout << "cut at " << cut << endl;
  }
}

//EFFECTS: Depending if the player won or less, adds or subtracts wager to the bankroll
int incrementBankroll(int bank, int wager, bool won)
{
  if(won)
    bank += wager;
  else
    bank -= wager;
  return bank;
}

//REQUIRES: pTotal and dTotal are <= 21
//EFFECTS: Determines whose total is higher, and calls incrementBankroll to change the bankroll
int compareTotals(int pTotal, int dTotal, int cash, int wager)
{
  if(pTotal == dTotal)
    cout << "Push" << endl;
  else if(pTotal > dTotal) {
    cout << "Player wins" << endl;
    cash = incrementBankroll(cash, wager, true);
  }
  else {
    cout << "Dealer wins" << endl;
    cash = incrementBankroll(cash, wager, false);
  }
  return cash;
}
      
//EFFECTS: Resturns the handValue of h
HandValue setHandValue(const Hand &h)
{
  return h.handValue();
}

//MODIFIES: The deck de, the hand h, and the player p
//EFFECTS: Deals a card to a player, exposing it and adding it to their hand
void dealToPlayer(Deck &de, Player *p, Hand &h)
{
  Card dealt;
  dealt = de.deal();
  h.addCard(dealt);
  p->expose(dealt);
  cout << "Player dealt " << SpotNames[dealt.spot] << " of " << SuitNames[dealt.suit] << endl;
}

//MODIFIES: The deck de, the dealer's hand h, and the player p
//EFFECTS: Deals a card to the dealer, and if appropriate, exposes it to the player
Card dealToDealer(Deck &de, Player *p, Hand &h, bool showCard)
{
  Card toDealer;
  toDealer = de.deal();
  h.addCard(toDealer);
  if(showCard) {
    p->expose(toDealer);
    cout << "Dealer dealt " << SpotNames[toDealer.spot] << " of " << SuitNames[toDealer.suit] << endl;
  }
  return toDealer;
}

int main(int argc, char *argv[])
{
  Player *p;
  Deck d;
  Hand plHand;
  Hand dealer;
  HandValue plValue, dealerValue;
  Card upCard, holeCard;   //To keep track of dealer's cards

  int bankroll = atoi(argv[1]);
  int rounds = atoi(argv[2]);
  int wager;

  string pType(argv[3]);
  
  //Get type of player
  if(pType == "simple")
    p = get_Simple();
  else if(pType == "counting")
    p = get_Counting();
  else if(pType == "competitor")
    p = get_Competitor();

  shuffleDeck(d);
  p->shuffled();

  int hand = 1;      // Increments number of hands, checks against rounds
  while(bankroll >= MINBET && hand <= rounds) {
    cout << "Hand " << hand << " bankroll " << bankroll << endl;

    int numLeft = d.cardsLeft();
    if(numLeft < 20) {
      shuffleDeck(d);
      p->shuffled();
    }
    wager = p->bet(bankroll, MINBET);
    cout << "Player bets " << wager << endl;
   
    dealToPlayer(d, p, plHand);
    upCard = dealToDealer(d, p, dealer, true);  //Dealt face up, announced to player
    dealToPlayer(d, p, plHand);
    holeCard = d.deal();
    dealer.addCard(holeCard);       //Not in function so it isn't exposed right away

    plValue = setHandValue(plHand);

    if(plValue.count == 21) {
      bankroll = bankroll + ((wager * 3) / 2);
      cout << "Player dealt natural 21" << endl;
    }
    else {  // Player plays
      while(p->draw(upCard, plHand)) {
	dealToPlayer(d, p, plHand);
      }
      plValue = setHandValue(plHand);
      cout << "Player's total is " << plValue.count << endl;
      if(plValue.count > 21) {
	cout << "Player busts" << endl;
	bankroll = incrementBankroll(bankroll, wager, false);
      }

      else {  //Dealer plays
	cout << "Dealer's hole card is " << SpotNames[holeCard.spot] << " of " << SuitNames[holeCard.suit] << endl;
	p->expose(holeCard);
	dealerValue = setHandValue(dealer);
	while(dealerValue.count < 17) {
	  upCard = dealToDealer(d, p, dealer, true);
	  dealerValue = setHandValue(dealer);
	}
	cout << "Dealer's total is " << dealerValue.count << endl;
	if(dealerValue.count > 21) {
	  cout << "Dealer busts" << endl;
	  bankroll = incrementBankroll(bankroll, wager, true);
	}
	else { //Dealer doesn't bust
	  bankroll = compareTotals(plValue.count, dealerValue.count, bankroll, wager);
	}
      }
    }
    plHand.discardAll();   //Reset the hands
    dealer.discardAll();
    hand++;
  }

  cout << "Player has " << bankroll << " after " << hand - 1 << " hands" << endl;

  return 0;
}
