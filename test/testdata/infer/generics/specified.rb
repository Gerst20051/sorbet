# typed: true
require_relative '../../t'

class PreChild < Parent
  extend T::Generic

  Elem = type_member {{fixed: String}}
end

class Parent
  extend T::Generic
  extend T::Sig

  Elem = type_member

  sig {params(a: Elem).returns(Elem)}
  def foo(a)
    a
  end
end

class Child < Parent
  extend T::Generic
  Elem = type_member {{fixed: String}}

  def use_foo
    T.assert_type!(foo("foo"), String)
  end
end

module Mixin
  extend T::Generic
  extend T::Sig
  Elem = type_member

  sig {params(a: Elem).returns(Elem)}
  def foo(a)
    a
  end
end

class WithMixin
  extend T::Generic
  include Mixin
  Elem = type_member {{fixed: String}}
end

class NotATypeVar
  NotAElem = String
end


class ParentWithMultiple
  extend T::Generic
  extend T::Sig

  K = type_member
  V = type_member

  sig {params(k: K, v: V).returns(K)}
  def foo(k, v)
    k
  end
end

class HalfChild < ParentWithMultiple
  extend T::Generic
  K = type_member {{fixed: Integer}}
  V = type_member
end

class HalfChildOther < ParentWithMultiple
  extend T::Generic
  K = type_member
  V = type_member {{fixed: Integer}}
end

class FullChild < HalfChild
  extend T::Generic
  extend T::Sig
  K = type_member {{fixed: Integer}}
  V = type_member {{fixed: String}}

  sig {params(f: FullChild).returns(FullChild[])}
  def f(f); f; end
end

class ParentEnumerable
  extend T::Generic
  include Enumerable

  K = type_member
  V = type_member
  Elem = type_member

  def each(&blk); end
end

class ChildEnumerable < ParentEnumerable
  extend T::Generic
  K = type_member {{fixed: String}}
  V = type_member
  Elem = type_member
end

def main
  a = Child.new.foo('a')
  T.assert_type!(a, String)

  a = PreChild.new.foo('a')
  T.assert_type!(a, String)

  a = WithMixin.new.foo('a')
  T.assert_type!(a, String)

  a = ParentWithMultiple[Symbol, String].new.foo(:a, 'b')
  T.assert_type!(a, Symbol)

  a = HalfChild[String].new.foo(1, 'b')
  T.assert_type!(a, Integer)

  a = HalfChildOther[String].new.foo('a', 1)
  T.assert_type!(a, String)

  a = FullChild.new.foo(1, 'b')
  T.assert_type!(a, Integer)

  a = ChildEnumerable[Integer, String].new.min
  T.assert_type!(a, T.nilable(String))
end
main
