module PCGL
  module Statement
    # simple rule definition
    # example:
    # my_rule
    class SimpleRule < Rule
      def initialize(line)
        super(line)
        return unless line =~ /\A([a-z_]+)\Z/i
        meta[:name] = Regexp.last_match(1)
        meta[:implicit] = false
      end

      def self.priority
        100
      end

      def self.can_parse?(line)
        indent_level(line).zero? && line.strip =~ /\A([a-z_]+)\Z/i
      end
    end
  end
end
