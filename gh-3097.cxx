#include <chrono>
#include <iostream>

int main() {
  try
  {
    const std::chrono::time_zone* current_z = std::chrono::current_zone();
    std::cout << "current timezone name: " << current_z->name() << "\n";
    auto sys_time = std::chrono::system_clock::now();
    std::cout << "current system time: " << sys_time << "\n";
    auto sys_info = current_z->get_info(sys_time); // throws exception what(): The operation completed successfully.
    (void)sys_info;
  }
  catch (const std::exception& ex)
  {
      std::cout << "caught exception: " << ex.what();
  }
}
