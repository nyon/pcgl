require './lib/pcgl/statement/rule'

module PCGL
  module Statement
    # conditional rule definition
    # example:
    # my_rule : width > 10
    class ImplicitRule < Rule
      def initialize(line)
        super(line)
        return unless line =~ /\A([a-z_]+)\s*:\s*(.*)\Z/i
        meta[:name] = Regexp.last_match(1)
        meta[:condition] = Regexp.last_match(2)
        meta[:implicit] = false
      end

      def self.priority
        100
      end

      def self.can_parse?(line)
        indent_level(line).zero? && line.strip =~ /\A([a-z_]+)\s*:\s*(.*)\Z/i
      end
    end
  end
end
