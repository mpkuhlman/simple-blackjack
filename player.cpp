#include "player.h"


class Simple:public Player {
 public:
  Simple();  //Simple constructor
  virtual int bet(unsigned int bankroll, unsigned int minimum);
  virtual bool draw(Card dealer, const Hand &player);
  virtual void expose(Card c);
  virtual void shuffled();
};

Simple::Simple()
{
}

int Simple::bet(unsigned int bankroll, unsigned int minimum)
{
  return minimum;
}

bool Simple::draw(Card dealer, const Hand &player)
{
  HandValue hv = player.handValue();
  //If player doesn't have an ace or the ace counts as one
  if(!hv.soft) {
    if(hv.count <= 11)
      return true;
    else if(hv.count == 12) {
      if(dealer.spot == FOUR || dealer.spot == FIVE || dealer.spot == SIX)
	return false;
      else
	return true;
    }
    else if(hv.count >=13 && hv.count <= 16) {
      if(dealer.spot >= TWO && dealer.spot <= SIX) 
	return false;
      else
	return true;
    }
    else
      return false;
  }
  //If player has an ace that counts as 11
  if(hv.count <= 17) 
    return true;
  else if(hv.count == 18) {
    if(dealer.spot == TWO || dealer.spot == SEVEN || dealer.spot == EIGHT)
      return false;
    else
      return true;
  }
  //Last possiblity, player has soft hand and has 19 or greater
  return false;
}

void Simple::expose(Card c)
{
}

void Simple::shuffled()
{
}

static Simple s;   

extern Player *get_Simple()
{
  return &s;
}

class Counting:public Simple {
 public:
  Counting();
  virtual int bet(unsigned int bankroll, unsigned int minimum);
  virtual void expose(Card c);
  virtual void shuffled();
  //uses same draw function as Simple player
 private:
  int countCard;
};

Counting::Counting()
{
  shuffled();
}

int Counting::bet(unsigned int bankroll, unsigned int minimum)
{
  if(countCard >= 2) {
    if(bankroll >= (2 * minimum))
      return 2 * minimum;
    else
      return bankroll;
  }
  return minimum;
}

void Counting::expose(Card c)
{
  if(c.spot >= TEN)
    countCard--;
  else if(c.spot <= SIX)
    countCard++;
}

void Counting::shuffled()
{
  countCard = 0;
}

static Counting counting;

extern Player *get_Counting()
{
  return &counting;
}

class Competitor:public Player {
 public:
  Competitor();
  virtual int bet(unsigned int bankroll, unsigned int minimum);
  virtual bool draw(Card dealer, const Hand &player);
  virtual void expose(Card c);
  virtual void shuffled();
 private:
  int aceCount;
  int otherCount;
  int getCount();
};

Competitor::Competitor()
{
  shuffled();
}

int Competitor::bet(unsigned int bankroll, unsigned int minimum)
{
  if(otherCount >= 5 || aceCount >= 20) {
    if(bankroll >= (3 * minimum))
      return 3 * minimum;
    else
      return bankroll;
  }
  else if((otherCount >= 2 && otherCount < 5) || (aceCount >= 15 && aceCount < 20)) {
    if(bankroll >= (2 * minimum))
      return 2 * minimum;
    else
      return bankroll;
  }
  return minimum;
}

bool Competitor::draw(Card dealer, const Hand &player) //Slight changes to Simple::draw involving count of cards
{
  int checkCount = getCount();
  HandValue hv = player.handValue();
  //If player doesn't have an ace or the ace counts as one
  if(!hv.soft) {
    if(hv.count <= 11)
      return true;
    else if(hv.count == 12) {
      if(dealer.spot == FOUR || dealer.spot == FIVE || dealer.spot == SIX)
        return false;
      else
        return true;
    }
    else if(hv.count >=13 && hv.count <= 16) {
      if(hv.count == 16 && dealer.spot == TEN && checkCount >= 1)
	return false;     //Different from simple and counting
      else if(hv.count == 15 && dealer.spot == TEN && checkCount >= 4)
	return false;     //Also different
      if(dealer.spot >= TWO && dealer.spot <= SIX)
        return false;
      else
        return true;
    }
    else
      return false;
  }
  //If player has an ace that counts as 11
  if(hv.count <= 17)
    return true;
  else if(hv.count == 18) {
    if(dealer.spot == TWO || dealer.spot == SEVEN || dealer.spot == EIGHT)
      return false;
    else
      return true;
  }
  //Last possiblity, player has soft hand and has 19 or greater
  return false;
}

void Competitor::expose(Card c)
{
  if(c.spot >= TEN)
    otherCount--;
  else if(c.spot <= SIX)
    otherCount++;
  if(c.spot != ACE)
    aceCount++;
  else
    aceCount = aceCount / 4;
}

void Competitor::shuffled()
{
  aceCount = 0;
  otherCount = 0;
}

//
int Competitor::getCount()
{
  return otherCount;
}

static Competitor comp;

extern Player *get_Competitor()
{
  return &comp;
}
