#include <optional>
#include "includes.hpp"
#include "helping_funcs.hpp"
#include "decimal.h"
// global vars
constexpr std::string_view PASSWORD_CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;':,./<>?";
constexpr std::string_view CODE_CHARSET = "23456789BCDFGHJKMNPQRSTVWXYZ";

std::mutex mtx;

struct Account;
struct User;
using account_database_alias = std::unordered_map<uint32_t, Account>;
using user_database_alias = std::unordered_map<std::string, User>;

using Money = dec::decimal<2, dec::half_up_round_policy>;
constexpr uint8_t DIVISOR = 100;
// main functionality
std::optional<std::string> generate_string(const std::string_view charset, const unsigned short length) {
  if (charset.empty()) {
    println("Error! Charset can't be empty!");
    return std::nullopt;
  }
  std::string generated_string;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> distrib(0, charset.length());
  for (size_t q = 0; q < length; ++q) {
    size_t generated_number = distrib(rd);
    generated_string.push_back(charset.at(generated_number));
  }
  return generated_string;
}

struct Account {
  std::string username;
  std::string password;
  std::optional<std::string> current_buyer;
  std::string game_name;

  Money price_per_hour;
  uint32_t id;
  bool is_free;

  std::optional<std::chrono::steady_clock::time_point> buy_time;
  std::optional<std::chrono::steady_clock::time_point> expiring_time;

  void null_acount() {
    current_buyer = std::nullopt;
    buy_time = std::nullopt;
    expiring_time = std::nullopt;
    is_free = true;
  }
};

struct User {
  std::string username;
  std::string password;
  Money balance;

  void top_up_balance(Money amount) {
    balance += amount;
  }
  void print_info() const {
    std::println("Username: {}\nBalance: {}", username, dec::toString(balance));
  }
};

void print_accounts(const account_database_alias &db) {
  for (auto &[id, acc] : db) {
    if (acc.is_free && acc.current_buyer == std::nullopt) {
      println("Id: {}\nPrice/hour: {}\nGame: {}", acc.id, dec::toString(acc.price_per_hour), acc.game_name);
    }
  }
}

void check_accounts(std::stop_token stop_token, account_database_alias &db) {
  while (!stop_token.stop_requested()) {
    {
      std::scoped_lock<std::mutex> lock(mtx);
      const auto now = std::chrono::steady_clock::now();
      for (auto &[id, acc] : db) {
        if (acc.is_free || !acc.current_buyer.has_value()) {
          continue;
        }

        if (acc.expiring_time && *acc.expiring_time < now) {
          acc.null_acount();
        }
      }
    }
    std::this_thread::sleep_for(5s);
  }
}

void register_user (user_database_alias &db, std::string_view username, std::string_view password) {
 if (auto it = db.find(static_cast<std::string>(username)); it != db.end()) {
  std::println("Error! User with this username already exists!");
  return;
 }
 User user = {static_cast<std::string>(username), static_cast<std::string>(password), Money(0)};
}

std::optional<std::string> login_user(const user_database_alias &db, std::string_view username, std::string_view password) {
  auto it = db.find(static_cast<std::string>(username));
  if (it == db.end()) {
    println("User with this username not found");
    return std::nullopt;
  }
  if (it->second.password == password) {
    return static_cast<std::string>(username);
  }
  println("Incorrect password");
  return std::nullopt;
}




int main() {
  account_database_alias account_db;
  user_database_alias user_db;
  std::jthread account_watcher(check_accounts, std::ref(account_db));

  return 0;
}
