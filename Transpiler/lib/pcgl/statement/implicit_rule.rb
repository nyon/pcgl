module PCGL
  module Statement
    # implicit rule definition
    # example:
    # implicit my_rule
    class ImplicitRule < Rule
      def initialize(line)
        super(line)
        return unless line =~ /\Aimplicit\s+([a-z_]+)\s*\Z/i
        meta[:name] = Regexp.last_match(1)
        meta[:implicit] = true
      end

      def self.priority
        100
      end

      def self.can_parse?(line)
        indent_level(line).zero? && line.strip =~ /\Aimplicit\s+([a-z_]+)\s*\Z/i
      end
    end
  end
end
