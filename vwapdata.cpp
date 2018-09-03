#pragma once

#include <stdio>
#include <stdint>
#include <stdlib>
#include <iostream>


struct Header
{
  uint8_t message_length;
  uint8_t message_type;
};

struct Quote
{
   uint8_t timestamp[8];
   char symbol[8];
   int8_t bid_price[4];
   uint8_t bid_qty[4];
   int8_t ask_price[4];
   uint8_t ask_qty[4];
   
   public:
   Quote() = default;
   ~Quote() = default;
   Quote(Quote&& other)
   {
      timestamp = other.timestamp;
      symbol = other.symbol;
      bid_price = other.bid_price;
      bid_qty = other.bid_qty;
      ask_price = other.ask_price;
      ask_qty = other.ask.qty;
   }
   
   Quote& operator=(Quote&& other)
   {
      if(this != &other)
      {
        timestamp = other.timestamp;
        symbol = other.symbol;
        bid_price = other.bid_price;
        bid_qty = other.bid_qty;
        ask_price = other.ask_price;
        ask_qty = other.ask.qty;
      }
      return *this;
   }
};

typedef std::shared_ptr<Quote*> QuotePtr;

struct Trade
{
  uint8_t timestamp[8];
  char symbol[8];
  int8_t price[4];
  unit8_t qty[4];
};

struct Order
{
  uint8_t timestamp[8];
  char symbol[8];
  char side;
  int8_t price[4];
  uint8_t qty[4];
};

typedef std::unordered_map<std::string, int> SIDEDATAMAP;

std::unordered_map<std::string, SIDEDATAMAP> vwapmap;

std::unordered_map<std::string, SIDEDATAMAP> avgprice;

std::unordered_map<std::srting, SIDEDATAMAP> totalqty;

std::unordered_map<std::string, std::shared_ptr<Quote*>> QuotePtrMap;

std::unordered_map<std::string, std::shared_ptr<Trade*>> TradePtrMap;


void updateQuote(std::unique_ptr<Quote> & ptr)  //ptr to a quote struct
{
    //search the hash map for the symbol and update the quote, if not found add this one
    auto pos = QuotePtrMap.find(ptr->symbol);
    if (pos != QuotePtrMap.end())
    {
        pos->second(std::move(ptr));
    }
    else
    {
      std::make_shared<Quote*>(ptr);
      QuotePtrMap.emplace(std::make_pair(ptr->symbol, ptr));
    }
}

void updatecounters(std::unique_ptr<Trade> & ptr)
{

    //update avg price
    auto pos = avgprice.find(ptr->symbol);
    if (pos != avgprice.end())
    {
      auto priceiter = pos->second.find(ptr->side);
      if(priceiter != pos->second.end())
      {
        auto oldprice = priceiter->second;
        auto currprice = ptr->price;
        auto newprice = (oldprice + newprice) / 2;
        avgprice[ptr->symbol][ptr->side] = newprice;
      }
      else
      {
        SIDEDATAMAP data;
        data.emplace(std::make_pair(ptr->side, ptr->price));
        avgprice.emplace(std::make_pair(ptr->symbol, data));
      }
    }
    else
    {
        SIDEDATAMAP data;
        data.emplace(std::make_pair(ptr->side, ptr->price));
        avgprice.emplace(std::make_pair(ptr->symbol, data));
    }
    
    //update total qty
    totalqty[ptr->symbol][ptr->side] += ptr->qty;
}

//getvwap
int getvwap(std::string symbol, std::string side)
{
    auto pos = vwapmap.find(symbol);
    if (pos != vwapmap.end())
    {
        auto inneriter = pos->second.find(side);
        if(inneriter != pos->second.end())
        {
          vwapmap[symbol][side] = (avgprice[symbol][side] * totalqty[symbol][side])/totalqty[symbol][side];
        }
    }
    else
    {
        auto vwap = (avgprice[symbol][side] * totalqty[symbol][side])/totalqty[symbol][side];
        SIDEDATAMAP data;
        data.emplace(std::make_pair(side, vwap));
        vwapmap.emplace(std::make_pair(symbol, data));
    }
    
    return vwapmap[symbol][side];
}

void sendOrder()
{
  //if current quote price is equal to or better than vwap, send an order
  
}

int main()
{
  for (int i =0; i < 100; i ++)
  {
     std::unique_ptr<Quote> qptr = std::make_unique<Quote>();
     qptr->timestamp = i;
     qptr->symbol = "BTC.USD";
     qptr->bid_price = i+10;
     qptr->bid_qty = i*100;
     qptr->ask_price = i + 11;
     qptr->ask_qty = i * 100;
     updateQuote(qptr);
  }
  
  for (int i =0; i < 10; i ++)
  {
     std::unique_ptr<Trade> tptr = std::make_unique<Trade>();
     tptr->timestamp = 10;
     tptr->symbol = "BTC.USD";
     tptr->price = 2*i+10;
     tptr->qty = i*100;
     updatecounters(qptr);
  }

  return 0;
};
