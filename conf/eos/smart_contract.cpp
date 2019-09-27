#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] hello : public contract {
  public:
      using contract::contract;

      [[eosio::action]]
i      void hi( int user ) {
         print( "Hello, ", user);
      }
};
