#include "boost/di/extension/injections/factory.hpp"

#include <cassert>

namespace di = boost::di;

struct interface {
  virtual ~interface() noexcept = default;
  virtual void dummy1() = 0;
};

struct implementation_with_injected_args : interface {
  implementation_with_injected_args(int i, double d) {
    assert(123 == i);
    assert(87.0 == d);
  }
  void dummy1() override {}
};

/*<<example `example` class>>*/
class example {
 public:
  example(const di::extension::ifactory<interface, double>& f3) {
    assert(dynamic_cast<implementation_with_injected_args*>(f3.create(87.0).get()));
  }
};

int main() {
  auto module = [] { return di::make_injector(di::bind<>().to(123)); };

  auto injector = di::make_injector(module()

  , di::bind<di::extension::ifactory<interface, double>>().to(di::extension::factory<implementation_with_injected_args>{})
  );

  /*<<create `example`>>*/
  injector.create<example>();
  injector.create<di::extension::ifactory<interface, double>&>();

}
