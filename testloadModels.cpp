#include "modelLoader/loadModels.hpp"

int main () {
  std::cout << "---------------------------------------------------------------\n";
  std::cout << "Loading Models...\n";
  loadModels();

  std::cout << "Saving Models...\n";
  std::ofstream out("models.bin", std::ios::binary);
  size_t size = models.size();
  out.write(reinterpret_cast<const char*>(&size), sizeof(size));
  for (const auto& m : models) {
      m.serialize(out);
  }
  std::cout << "Done!\n";
  std::cout << "---------------------------------------------------------------\n";

  return 0;
}