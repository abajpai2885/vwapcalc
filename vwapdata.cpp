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

typedef std::shared_ptr<Quote*>

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

std::unordered_map<std::string, int> vwapmap;

std::unordered_map<std::string, int> avgprice;

std::unordered_map<std::srting, int> totalqty;

std::unordered_map<std::string, std::shared_ptr<Quote*>> QuotePtrMap;

std::unordered_map<std::string, std::shared_ptr<Trade*>> TradePtrMap;


void updateQuote(Quote* ptr)  //ptr to a quote struct
{
    //search the hash map for the symbol and update the quote, if not found add this one
    auto pos = QuotePtrMap.find(ptr->symbol);
    if (pos != QuotePtrMap.end())
    {
        pos->second(std::move(ptr));
    }
    else
    {
      std::make_shared<Quoet*>(ptr);
      QuotePtrMap.emplace(std::make_pair(ptr->symbol, ptr));
    }
}

void updatecounters(Trade* ptr)
{

    //update avg price
    auto pos = avgprice.find(ptr->symbol);
    if (pos != avgprice.end())
    {
      auto oldprice = pos->second;
      auto currprice = ptr->price;
      auto newprice = (oldprice + newprice) / 2;
      avgprice[ptr->symbol] = newprice;
    }
    else
    {
        avgprice.emplace(std::make_pair(ptr->symbol, ptr->price));
    }
    
    //update total qty
    pos = totalqty.find(ptr->symbol);
    if(pos != totalqty.end())
    {
      totalqty[ptr->symbol] += ptr->qty;
    }
    else
    {
      totalqty.emplace(std::make_pair(ptr->symbol, ptr->qty));
    }
}

int vwap(std::string symbol)
{
    auto pos = vwapmap.find(symbol);
    if (pos != vwapmap.end())
    {
        vwapmap[symbol] = (avgprice[symbol] * totalqty[symbol])/totalqty[symbol];
    }
    else
    {
        auto vwap = (avgprice[symbol] * totalqty[symbol])/totalqty[symbol];
        vwapmap.emplace(std::make_pair(symbol, vwap));
    }
    
    return vwapmap[symbol];
}
