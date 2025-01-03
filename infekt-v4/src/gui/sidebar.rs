use iced::widget::{button, column, container, image, row, svg, text, Space};
use iced::Length::{self, Fill};
use iced::{Alignment, Element, Theme};

use crate::app::{InfektActiveScreen, InfektAppAction};

#[derive(Default)]
pub struct InfektSidebar {
    expanded: bool,
}

#[derive(Debug, Clone)]
pub enum InfektSidebarMessage {
    ToggleSidebar,
    ShowMainView,
    ShowPreferences,
    ShowAboutScreen,
    OpenFileDialog,
}

const EXPANDED_WIDTH: Length = Length::Fixed(200.0);
const COLLAPSED_WIDTH: Length = Length::Fixed(50.0);

const LOGO_256: &[u8] = include_bytes!("../../assets/infekt-icons/iNFekt_6_256x256x32.png");
const EXPAND_ICON: &[u8] =
    include_bytes!("../../assets/tabler-icons/outline/layout-sidebar-left-expand.svg");
const COLLAPSE_ICON: &[u8] =
    include_bytes!("../../assets/tabler-icons/outline/layout-sidebar-left-collapse.svg");

impl InfektSidebar {
    pub fn update(&mut self, message: InfektSidebarMessage) -> InfektAppAction {
        match message {
            InfektSidebarMessage::ToggleSidebar => {
                self.expanded = !self.expanded;
                InfektAppAction::None
            }
            InfektSidebarMessage::ShowMainView => {
                InfektAppAction::ShowScreen(InfektActiveScreen::MainView)
            }
            InfektSidebarMessage::ShowPreferences => {
                InfektAppAction::ShowScreen(InfektActiveScreen::Preferences)
            }
            InfektSidebarMessage::ShowAboutScreen => {
                InfektAppAction::ShowScreen(InfektActiveScreen::About)
            }
            InfektSidebarMessage::OpenFileDialog => InfektAppAction::SelectFileForOpening,
        }
    }

    pub fn view(&self) -> Element<InfektSidebarMessage> {
        let column = column![
            container(self.logo()).center_x(Fill).center_y(48.0),
            self.top_level_button(
                "Home",
                include_bytes!("../../assets/tabler-icons/outline/home.svg"),
                InfektSidebarMessage::ShowMainView
            ),
            self.top_level_button(
                "Open...",
                include_bytes!("../../assets/tabler-icons/outline/folder-open.svg"),
                InfektSidebarMessage::OpenFileDialog
            ),
            self.top_level_button(
                "Preferences",
                include_bytes!("../../assets/tabler-icons/outline/settings.svg"),
                InfektSidebarMessage::ShowPreferences
            ),
            self.top_level_button(
                "About",
                include_bytes!("../../assets/tabler-icons/outline/info-hexagon.svg"),
                InfektSidebarMessage::ShowAboutScreen
            ),
            Space::with_height(Fill),
            button(svg(self.toggle_icon()))
                .width(Fill)
                .on_press(InfektSidebarMessage::ToggleSidebar),
        ]
        .spacing(1);

        container(column)
            .style(|theme: &Theme| {
                let palette = theme.extended_palette();

                container::Style::default().background(palette.background.strong.color)
            })
            .height(Fill)
            .width(self.current_width())
            .into()
    }

    fn current_width(&self) -> Length {
        if self.expanded {
            EXPANDED_WIDTH
        } else {
            COLLAPSED_WIDTH
        }
    }

    fn toggle_icon(&self) -> svg::Handle {
        svg::Handle::from_memory(if self.expanded {
            COLLAPSE_ICON
        } else {
            EXPAND_ICON
        })
    }

    fn logo(&self) -> Element<'static, InfektSidebarMessage> {
        if self.expanded {
            text("iNFekt").size(26).into()
        } else {
            image(image::Handle::from_bytes(LOGO_256))
                .height(24.0)
                .into()
        }
    }

    fn top_level_button(
        &self,
        label: &'static str,
        icon_bytes: &'static [u8],
        message: InfektSidebarMessage,
    ) -> Element<'static, InfektSidebarMessage> {
        let icon_handle = svg::Handle::from_memory(icon_bytes);
        let icon_widget = svg(icon_handle).height(24.0).width(24.0);

        let btn = if self.expanded {
            let text_widget = text(label);

            button(
                row![icon_widget, text_widget]
                    .spacing(8)
                    .align_y(Alignment::Center),
            )
        } else {
            button(container(icon_widget).center_x(Fill))
        };

        btn.width(Fill).on_press(message).into()
    }
}
