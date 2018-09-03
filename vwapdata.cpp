#include <iostream>
#include <cstdlib>
#include <memory>
#include <unordered_map>


struct Header
{
  uint8_t message_length;
  uint8_t message_type;
};

//normalized quote struct
//assumption : innbound message quote message will be converted to this
struct Quote
{
   long timestamp;
   std::string symbol;
   double bid_price;
   double bid_qty;
   double ask_price;
   double ask_qty;
   
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
      ask_qty = other.ask_qty;
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
        ask_qty = other.ask_qty;
      }
      return *this;
   }
};

typedef std::shared_ptr<Quote*> QuotePtr;

//normalized Trade struct
//assumption: inbound Trade message will be nrmalized into this struct
struct Trade
{
  long timestamp;
  std::string symbol;
  double price;
  double qty;
};

//normalized Order message
struct Order
{
  long timestamp;
  std::string symbol;
  std::string side;
  double price;
  double qty;
};

typedef std::unordered_map<std::string, int> SIDEDATAMAP;

std::unordered_map<std::string, SIDEDATAMAP> vwapmap;

std::unordered_map<std::string, SIDEDATAMAP> avgprice;

std::unordered_map<std::string, SIDEDATAMAP> totalqty;

std::unordered_map<std::string, std::shared_ptr<Quote*>> QuotePtrMap;

std::unordered_map<std::string, std::shared_ptr<Trade*>> TradePtrMap;


void updateQuote(std::unique_ptr<Quote> & ptr)  //ptr to a quote struct
{
    //search the hash map for the symbol and update the quote, if not found add this one
    auto pos = QuotePtrMap.find(ptr->symbol);
    if (pos != QuotePtrMap.end())
    {
        pos->second(std::move(std::make_shared(ptr)));
    }
    else
    {
      std::make_shared<Quote>(ptr);
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

