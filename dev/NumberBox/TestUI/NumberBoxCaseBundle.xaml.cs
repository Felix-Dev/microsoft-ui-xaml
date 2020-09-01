namespace MUXControlsTestApp
{
    [TopLevelTestPage(Name = "NumberBox")]
    public sealed partial class NumberBoxCaseBundle : TestPage
    {
        public NumberBoxCaseBundle()
        {
            this.InitializeComponent();

            NumberBoxPageButton.Click += delegate { Frame.NavigateWithoutAnimation(typeof(NumberBoxPage), 0); };
            NumberBoxNumberFormatterPageButton.Click += delegate { Frame.NavigateWithoutAnimation(typeof(NumberBoxNumberFormatterPage), 0); };
        }
    }
}
