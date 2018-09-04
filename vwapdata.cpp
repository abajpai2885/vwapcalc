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
//assumption : innbound message quote message will be normalized to this format
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
   Quote(Quote& other) = default;
   Quote& operator=(Quote& other) = default;
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

//normalized Trade struct
//assumption: inbound Trade message will be nrmalized into this struct
//assumption: trade message also has a "side" property in it because quantiy is unsigned, so there is 
//no other way of knowing the type of trade executed
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


std::unordered_map<std::string, double> vwapmap;

std::unordered_map<std::string, double> avgprice;

std::unordered_map<std::string, long> totalqty;

std::unordered_map<std::string, std::shared_ptr<Quote>> QuotePtrMap;

std::unordered_map<std::string, std::shared_ptr<Trade>> TradePtrMap;


void updateQuote(std::unique_ptr<Quote> & ptr)  //ptr to a quote struct
{
    //search the hash map for the symbol and update the quote, if not found add this one
    auto pos = QuotePtrMap.find(ptr->symbol);
    if (pos != QuotePtrMap.end())
    {
       pos->second = std::make_shared<Quote>(*ptr);
    }
    else
    {
      //std::make_shared<Quote>(ptr.get());
      QuotePtrMap.emplace(std::make_pair(ptr->symbol, std::make_shared<Quote>(*(ptr))));
    }
}

void updatecounters(std::unique_ptr<Trade> & ptr)
{

    //update avg price
    auto pos = avgprice.find(ptr->symbol);
    if (pos != avgprice.end())
    {
        auto oldprice = pos->second;
        auto currprice = ptr->price;
        avgprice[ptr->symbol] = (oldprice + currprice) / 2;
    }
    else
        avgprice.emplace(std::make_pair(ptr->symbol, ptr->price));
    
    //update total qty
    totalqty[ptr->symbol] += ptr->qty;
     
}

//getvwap
double calcvwap(std::string symbol)
{
    auto pos = vwapmap.find(symbol);
    if (pos != vwapmap.end())
    {
          vwapmap[symbol] = (avgprice[symbol] * totalqty[symbol])/totalqty[symbol];
    }
    else
        vwapmap.emplace(std::make_pair(symbol, (avgprice[symbol] * totalqty[symbol])/totalqty[symbol]));
    
    return vwapmap[symbol];
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
     updatecounters(tptr);
     double vwap = calcvwap("BTC.USD");
     std::cout << "VWAP is : " << vwap << std::endl;
  }

  return 0;
};
