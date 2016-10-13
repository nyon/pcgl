module PCGL
  module Statement
    # command class for pcgl
    # example:
    # my_command parameter1, parameter2, parameter3
    class Command < Base
      def initialize(line)
        super(line)

        statement = line.strip
        statement =~ Command.regex
        meta[:name] = Regexp.last_match(1)
        parameter_list = Regexp.last_match(2) || ''
        meta[:parameter] = parameter_list.strip.split(',').map(&:strip)
      end

      def self.priority
        100
      end

      def self.regex
        /\A([a-z_]+)(\s+(.*))?\Z/i
      end

      def self.can_parse?(line)
        statement = line.strip
        indent_level(line).nonzero? && statement =~ regex
      end
    end
  end
end
