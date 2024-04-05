import styled from "styled-components/native";

const Wrapper = styled.View`
  height: 32px;
  flex-direction: row;
  justify-content: space-between;


    border-radius: 9999px;
    border: #7BE4A5 solid 2px;
    
    margin-top: 16px;
    margin-bottom: 24px;
`

const Label = styled.View`
  height: 100%;
    background: #7BE4A5;
    justify-content: center;
    border-radius: 9999px;
    padding-left: 24px;
    padding-right: 16px;
`

const LabelText = styled.Text`
font-size: 20px;
font-weight: bold;
color: #FFF;
`

const Badge = styled.View`
    justify-content: center;
    padding-right: 16px;
`

const BadgeText = styled.Text`
    font-size: 20px;
    font-weight: bold;
    color: #7BE4A5;
`

export const ConnectedDeviceBadge = ({ label, deviceName }: { label: string, deviceName: string}) => (
  <Wrapper>
    <Label>
      <LabelText>{label}</LabelText>
    </Label>
    <Badge>
      <BadgeText>{deviceName}</BadgeText>
    </Badge>
  </Wrapper>
)