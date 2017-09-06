require_relative './base'

module PCGL
  module Statement
    # rule definition base
    class Rule < Base
      def initialize(line)
        super(line)
        self.meta = {
          name: 'rule',
          implicit: false,
          condition: false
        }
      end

      def self.priority
        100
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
