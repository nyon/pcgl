module PCGL
  module Statement
    # root statement for the statement tree
    class Root < Base
      def self.priority
        1000
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
