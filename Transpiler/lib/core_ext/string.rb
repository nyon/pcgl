# Enhances the string class with transformation methods:
# - demodulize
# - to_snakecase
# - to_camelcase
# - lchomp
class String
  def demodulize
    split('::').last
  end

  def to_snakecase
    gsub(/[A-Z]/) do |match|
      "_#{match.downcase}"
    end.lchomp('_')
  end

  def to_camelcase
    gsub(/(\A|_)[a-z]/, &:upcase)
  end

  def lchomp(match)
    if index(match).zero?
      self[match.size..-1]
    else
      dup
    end
  end
end
