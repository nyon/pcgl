# Enhances all objects with try methods
# borrowed from rails:
# https://github.com/rails/rails/blob/be589a8b01500a4b52a86248458bf57597e568e8/activesupport/lib/active_support/core_ext/object/try.rb#L62
class Object
  def try(*a, &b)
    try!(*a, &b) if a.empty? || respond_to?(a.first)
  end

  def try!(*a, &b)
    if a.empty? && block_given?
      if b.arity.zero?
        instance_eval(&b)
      else
        yield self
      end
    else
      public_send(*a, &b)
    end
  end
end
