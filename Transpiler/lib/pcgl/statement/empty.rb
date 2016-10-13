module PCGL
  module Statement
    # empty statement, represents a blank line
    class Empty < Base
      def self.priority
        0
      end

      def self.can_parse?(line)
        line.strip.empty?
      end
    end
  end
end
