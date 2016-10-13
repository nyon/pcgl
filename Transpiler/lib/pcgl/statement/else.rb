module PCGL
  module Statement
    # else stub
    class Else < Base
      def self.priority
        0
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
