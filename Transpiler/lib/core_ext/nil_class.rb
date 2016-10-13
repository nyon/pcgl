# Enhances the nil class with try methods
class NilClass
  def try(*_args)
    nil
  end

  def try!(*_args)
    nil
  end
end
